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
#include <ppapi/cpp/core.h>
#include <ppapi/c/ppb_image_data.h>
using namespace pp;
#endif

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QPepperWindowSurface::QPepperWindowSurface(QWidget *window)
    : QWindowSurface(window)
{
    m_platformWindow = static_cast<QPepperPlatformWindow *>(window->platformWindow());
    m_pepperInstance = 0;
    m_frameBuffer = 0;
    m_ownsFrameBuffer = false;
    m_isInPaint = false;
    m_compositor = &QtPepperMain::get()->m_compositor;

   // qDebug() << "QPepperWindowSurface::QPepperWindowSurface" << m_platformWindow->windowId();

    m_compositor->setWindowSurface(m_platformWindow, this);

//    createFrameBuffer(window->size());
}

QPepperWindowSurface::~QPepperWindowSurface()
{
    // qDebug() << "QPepperWindowSurface::~QPepperWindowSurface" << window();
    if (m_ownsFrameBuffer)
        delete m_frameBuffer;
}

void QPepperWindowSurface::createFrameBuffer(QSize size)
{
    if (!this->size().isValid())
        return;
   // qDebug() << "QPepperWindowSurface::createFrameBuffer" << size;

    if (m_ownsFrameBuffer)
        delete m_frameBuffer;
    m_frameBuffer = new QImage(size, QImage::Format_ARGB32_Premultiplied);
    m_frameBuffer->fill(Qt::red); // should never be seen
    m_ownsFrameBuffer = true;

    m_compositor->setFrameBuffer(this,  m_frameBuffer);

}

void QPepperWindowSurface::setFrameBuffer(QImage *frameBuffer)
{
//    if (m_isInPaint)
        qFatal("QPepperWindowSurface::setFrameBuffer called.");

    if (m_ownsFrameBuffer)
        delete m_frameBuffer;
    m_frameBuffer = frameBuffer;
    m_ownsFrameBuffer = false;

    window()->resize(frameBuffer->size());
}

void QPepperWindowSurface::setPepperInstance(QPepperInstance *instance)
{
    m_pepperInstance = instance;
}

QPaintDevice *QPepperWindowSurface::paintDevice()
{
//    qDebug() << "QPepperWindowSurface::paintDevice" << m_frameBuffer;
    if (m_frameBuffer == 0)
        qFatal("QPepperWindowSurface::paintDevice frameBuffer is 0");
    return m_frameBuffer;
}

void QPepperWindowSurface::beginPaint(const QRegion &region)
{
    // qDebug() << "QPepperWindowSurface::beginPaint" << window();
    m_isInPaint = true;

    m_compositor->waitForFlushed(this);

    QWindowSurface::beginPaint(region);
}

void QPepperWindowSurface::endPaint(const QRegion &region)
{
//    qDebug() << "QPepperWindowSurface::endPaint";
    QWindowSurface::endPaint(region);
    m_isInPaint = false;
}

bool QPepperWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    Q_UNUSED(area);
    Q_UNUSED(dx);
    Q_UNUSED(dy);
    return false;
}

void QPepperWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget); // ### TODO: complications if widget
                      // is a child widget. (see QWindowSurface::flush)
    Q_UNUSED(region);
    Q_UNUSED(offset);

    //qDebug() << "QPepperWindowSurface::flush" << this->window();

    m_compositor->flush(this);
}

void QPepperWindowSurface::resize(const QSize &size)
{
    QWindowSurface::resize(size);
  //  qDebug() << "resize" << size;
  //  if (m_frameBuffer)
  //      qDebug() << "resize" << m_frameBuffer->size();


    if (!m_frameBuffer || size != m_frameBuffer->size())
        createFrameBuffer(size);
}

QT_END_NAMESPACE
