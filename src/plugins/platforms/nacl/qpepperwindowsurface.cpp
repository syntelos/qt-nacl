/*
    Licensed under the MIT license.
*/
#include "qpepperwindowsurface.h"
#include "qpepperplatformwindow.h"
#include "qpeppermain.h"
#include "peppermodule.h"

#include <QWindowSystemInterface>
#include <QPainter>

#ifndef QT_NO_PEPPER_INTEGRATION
#include <ppapi/cpp/graphics_2d.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/core.h>
#include <ppapi/c/ppb_image_data.h>
using namespace pp;
#endif

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QPepperWindowSurface::QPepperWindowSurface(QWidget *window, QPepperWindowSurface *parentWindowSurface)
    : QWindowSurface(window)
{
    // qDebug() << "QPepperWindowSurface::QPepperWindowSurface" << window << parentWindowSurface;
    m_parentWindowSurface = parentWindowSurface;
    m_childWindowPos = window->pos();
    if (m_parentWindowSurface)
        m_parentWindowSurface->m_childWindowSurfaces.push_back(this);
    createQImagePaintDevice(window->size());
}

QPepperWindowSurface::~QPepperWindowSurface()
{
    // qDebug() << "QPepperWindowSurface::~QPepperWindowSurface" << window();
    if (m_parentWindowSurface)
        m_parentWindowSurface->m_childWindowSurfaces.removeAll(this);
}

#ifdef QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION
void qPepperWindowSurfaceCreatePepperGraphicsContext(void *context)
{
    reinterpret_cast<QPepperWindowSurface *>(context)->createPepperGraphicsContext();
}

void QPepperWindowSurface::createPepperGraphicsContext()
{
    qDebug() << "createPepperGraphicsContext_callback" << QThread::currentThreadId() << QtModule::getCore()->IsMainThread();

    QtPepperMain *pepperMain = QtPepperMain::globalInstance();
    qDebug() << "createPepperGraphicsContext_callback  2" << QtModule::getCore()->IsMainThread();

    m_graphicsContext = Graphics2D(pepperMain->instance(), toPPSize(this->size()), false);
    qDebug() << "createPepperGraphicsContext_callback  3" << QtModule::getCore()->IsMainThread();

    if (!pepperMain->instance()->BindGraphics(m_graphicsContext)) {
        qDebug() << "Couldn't bind the device context";
    }
    qDebug() << "createPepperGraphicsContext_callback  4";

    m_imageData = ImageData(pepperMain->instance(),
                           PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                           toPPSize(this->size()), true);

    m_graphicsContextCreated = true;
    qDebug() << "createPepperGraphicsContext_callback done";
}
#endif

void QPepperWindowSurface::createQImagePaintDevice(QSize size)
{
    QtPepperMain *pepperMain = QtPepperMain::globalInstance();

    if (!this->size().isValid())
        return;

    if (window()->testAttribute(Qt::WA_DontShowOnScreen)) {
        // This code path is used by QStyle for pixmap cashing
        m_qtImage = QImage(size, QImage::Format_ARGB32_Premultiplied);
    } else if (m_parentWindowSurface) {
        // Child window surfaces:
        m_qtImage = QImage(window()->size(), QImage::Format_ARGB32_Premultiplied);

    } else {
#ifdef QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION
        qDebug() << "QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION 1" << QThread::currentThreadId() << QtModule::getCore()->IsMainThread();;
        m_graphicsContextCreated = false;
        pepperMain->runOnPepperThread(qPepperWindowSurfaceCreatePepperGraphicsContext, this);
        // Busy-wait until the contexts has been created on the pepper thread.
        // ### Replace with runOnPepperThreadSynchronous call when that is stable.

        qDebug() << "QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION 1 delay";
        while (!m_graphicsContextCreated)
            ;

        qDebug() << "QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION 1 delay done";
        // Use the pepper ImageData as the backing buffer.
        m_qtImage = QImage(reinterpret_cast<uchar *>(m_imageData.data()),
                       size.width(), size.height(),
                       m_imageData.stride(),
                       QImage::Format_ARGB32_Premultiplied);
        qDebug() << "QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION done 12";

#else
        // Use the pepper ImageData as the backing buffer.
        m_qtImage = QImage(reinterpret_cast<uchar *>(pepperMain->m_imageData.data()),
                       size.width(), size.height(),
                       pepperMain->m_imageData.stride(),
                       QImage::Format_ARGB32_Premultiplied);

#endif
    }
}

QPaintDevice *QPepperWindowSurface::paintDevice()
{
    //qDebug() << "QPepperWindowSurface::paintDevice";
    return &m_qtImage;
}

void QPepperWindowSurface::beginPaint(const QRegion &region)
{
//    qDebug() << "QPepperWindowSurface::beginPaint";
    if (m_parentWindowSurface)
        return;

    QtPepperMain *pepperMain = QtPepperMain::globalInstance();
    {
        QMutexLocker lock(&pepperMain->m_mutex);
        // Check if the pepper thread is flushing the backing
        // buffer. If this is the case the Qt thread cannot
        // start painting to it yet - this will cause flickering
        // if pepper flushes a half-painted frame.
        if (pepperMain->m_inFlush) {
            pepperMain->m_qtWaiting = true;

            // If there is a pending resize the pepper thread might be waiting
            // for the Qt thread to sleep. Wake it.
            if (pepperMain->m_screenResized) {
                pepperMain->m_pepperWait.wakeOne();
            }

            // Wait until pepper is done flushing. This will block the
            // qt thread and prevent it from processing envets. Use
            // timers to rate limit painting to avoid this!
            pepperMain->m_qtWait.wait(&pepperMain->m_mutex);
            pepperMain->m_qtWaiting = false;
        }
    }
    QWindowSurface::beginPaint(region);
}

void QPepperWindowSurface::endPaint(const QRegion &region)
{
    if (m_parentWindowSurface)
        return;

    //    qDebug() << "QPepperWindowSurface::endPaint";
    QWindowSurface::endPaint(region);
}

bool QPepperWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    Q_UNUSED(area);
    Q_UNUSED(dx);
    Q_UNUSED(dy);
    return false;
}

// flush callback called on the pepper thread when ppepper is
// done flushing the backing buffer to screen
void flushCallback(void* data, int32_t result)
{
    Q_UNUSED(data);
    Q_UNUSED(result);

    QtPepperMain *pepperMain = QtPepperMain::globalInstance();
    QMutexLocker lock(&pepperMain->m_mutex);
    pepperMain->m_inFlush = false;
    pepperMain->m_qtWait.wakeOne();
}

// flush function called on the pepper thread.
void flushFunction(void* context)
{
    QtPepperMain *pepperMain = QtPepperMain::globalInstance();
#ifdef QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION
    qDebug() << "QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION 13";
    QPepperWindowSurface *windowSurface = reinterpret_cast<QPepperWindowSurface *>(context);
    if (windowSurface->m_graphicsContext.is_null()) {
        pepperMain->m_inFlush = false;
        return;
    }

    windowSurface->m_graphicsContext.PaintImageData(windowSurface->m_imageData, pp::Point(),
                                                 pp::Rect(0, 0, windowSurface->m_graphicsContext.size().width(),
                                                                windowSurface->m_graphicsContext.size().height()));
    windowSurface->m_graphicsContext.Flush(CompletionCallback(&flushCallback, 0));
    qDebug() << "QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION 14";
#else
    if (pepperMain->m_graphicsContext.is_null()) {
        pepperMain->m_inFlush = false;
        return;
    }

    pepperMain->m_graphicsContext.PaintImageData(pepperMain->m_imageData, pp::Point(),
                                                 pp::Rect(0, 0, pepperMain->m_graphicsContext.size().width(),
                                                                pepperMain->m_graphicsContext.size().height()));
    pepperMain->m_graphicsContext.Flush(CompletionCallback(&flushCallback, 0));

#endif
}

void QPepperWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget); // ### TODO: complications if widget
                      // is a child widget. (see QWindowSurface::flush)
    Q_UNUSED(region);
    Q_UNUSED(offset);

    qDebug() << "QPepperWindowSurface::flush" << QtModule::getCore()->IsMainThread();

    // composit child window surfaces onto this surface.
    foreach (QPepperWindowSurface *childWindowSurface, m_childWindowSurfaces) {
        QPainter p(&m_qtImage);
        if (childWindowSurface->window()->isVisible())
            p.drawImage(childWindowSurface->window()->pos(), childWindowSurface->m_qtImage);
    }

    // If the windowsurface has a parent it has no pepper graphics context,
    // instead flush the parent context
    if (m_parentWindowSurface) {
        m_parentWindowSurface->flush(0, QRegion(), QPoint());
        return;
    }

#ifdef QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION
    qDebug() << "QPepperWindowSurface::flush ##";
    if (m_graphicsContext.is_null())
        return;
    qDebug("QPepperWindowSurface::done ###");
#else
    if (QtPepperMain::globalInstance()->m_graphicsContext.is_null())
        return;
#endif
    // Schedule the flush on the pepper thread.
    QtPepperMain *pepperMain = QtPepperMain::globalInstance();
    if (pepperMain->m_exitNow)
        return;
    pepperMain->m_inFlush = true;
    pepperMain->runOnPepperThread(flushFunction, this);
}

void QPepperWindowSurface::resize(const QSize &size)
{
    qDebug() << "QPepperWindowSurface::resize";
    QWindowSurface::resize(size);
    createQImagePaintDevice(size);
}

#ifndef QT_NO_PEPPER_INTEGRATION
pp::Rect QPepperWindowSurface::pepperRect(const QRect rect)
{
    return pp::Rect(rect.x(), rect.y(), rect.size().width(), rect.size().height());
}
#endif


QT_END_NAMESPACE
