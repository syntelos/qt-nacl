/*
    Licensed under the MIT license.
*/
#include "pepperinstance.h"
#include <stdio.h>

#ifndef QT_PEPPER_STANDALONE_MODE
#include "qpeppermain.h"
#include "qpepperhelpers.h"
#include "qpepperwindowsurface.h"
#include "peppermodule.h"
#include <QWindowSystemInterface>
#endif

#include "ppapi/cpp/var.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/completion_callback.h"

using namespace pp;

void flush_callback(void *, int32_t)
{

}

QPepperInstance::QPepperInstance(PP_Instance instance)
: pp::Instance(instance)
{
    m_currentGeometry = Rect();
    m_context2D = 0;
    m_imageData2D = 0;
    m_frameBuffer = 0;
    m_inFlush = false;
    m_callbackFactory.Initialize(this);
}

QPepperInstance::~QPepperInstance()
{
    delete m_context2D;
    delete m_imageData2D;
    delete m_frameBuffer;

    QtPepperMain::get()->setInstance(0);
}

bool QPepperInstance::Init(uint32_t argc, const char* argn[], const char* argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argn);
    Q_UNUSED(argv);

    m_windowId = 0;
    for (int i = 0; i < argc; ++i) {
        //qDebug() << "arg" << argn[i] << argv[i];
        if (qstrcmp(argn[i], "qt_window_id") == 0) {
            m_windowId = QByteArray(argv[i]).toInt();
        }
    }

    // qDebug() << "PepperInstance::init" << m_windowId;

    RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL | PP_INPUTEVENT_CLASS_KEYBOARD);
    QtPepperMain::get()->setInstance(this); // ### pass arguments?

    return true;
}

void QPepperInstance::DidChangeView(const Rect& geometry, const Rect& clip)
{
    Q_UNUSED(clip);

    if (geometry.size() == m_currentGeometry.size())
        return;
    m_currentGeometry = geometry;

   // qDebug() << "QPepperInstance::DidChangeView" << m_windowId << geometry.size().width() << geometry.size().height();

    QtPepperMain *pepperMain = QtPepperMain::get();

    // Delete the previous framebuffer and graphics context. Park the Qt
    // thread first, to make sure we don't remove the frame buffer while
    // its in use.
    pepperMain->parkQtThread();
    delete m_context2D;
    delete m_imageData2D;
    delete m_frameBuffer;

    // Create new graphics context and frame buffer.
    m_context2D = new Graphics2D(this, geometry.size(), false);
    if (!BindGraphics(*m_context2D)) {
        printf("Couldn't bind the device context\n");
    }

    m_imageData2D = new ImageData(this, PP_IMAGEDATAFORMAT_BGRA_PREMUL, geometry.size(), true);
    m_frameBuffer = new QImage(reinterpret_cast<uchar *>(m_imageData2D->data()),
                               geometry.size().width(), geometry.size().height(),
                               m_imageData2D->stride(), QImage::Format_ARGB32_Premultiplied);

    // Set the frame buffer on the compositor
    pepperMain->m_compositor.setRasterFrameBuffer(m_frameBuffer);
    pepperMain->m_compositor.composit();

    // Wake Qt again
    QWindowSystemInterface::handleScreenGeometryChange(0);
    pepperMain->resumeQtThread();
//    qDebug() << "QPepperInstance::DidChangeView done";
}

void QPepperInstance::DidChangeFocus(bool has_focus)
{
    Q_UNUSED(has_focus);
}

bool QPepperInstance::HandleInputEvent(const pp::InputEvent& event)
{
    QtPepperMain *pepperMain = QtPepperMain::get();

    if (pepperMain->m_qtReadyForEvents == false)
        return false;

    // Translate and post event to Qt via the compositor
    bool ret = m_eventTranslator.processEvent(event);

    // The Qt thread is most likely waiting on a timer timeout,
    // wake it so it can process the event.
    pepperMain->qtThreadWake();
    return ret;
}

bool QPepperInstance::HandleDocumentLoad(const URLLoader& url_loader)
{
    Q_UNUSED(url_loader);
    return false;
}

// HandleMessage expects a message of the format "funcitonName:argument", or
// "functionName" and will call the corresponding slot on qtScriptableObject.
void QPepperInstance::HandleMessage(const Var& var_message)
{
    extern QObject *qtScriptableObject; //qglobal.cpp
    if (qtScriptableObject == 0)
        return;

    QString message = QString::fromUtf8(var_message.AsString().data());

    QStringList parts = message.split(QLatin1Char(':'));
    if (parts.count() == 0)
        return; // Empty or ill-formed message

    const char *methodName = parts.at(0).toLatin1().constData();

    // Call the function on the Qt main thread using Qt::QueuedConnection since
    // this HandleMessage is called from the pepper tread. Currently zero or one
    // arguments are supported.
    if (parts.count() == 1) {
        QMetaObject::invokeMethod(qtScriptableObject, methodName, Qt::QueuedConnection);
    } else if (parts.count() == 2) {
        QString argument = parts.at(1);
        QMetaObject::invokeMethod(qtScriptableObject, methodName, Qt::QueuedConnection, Q_ARG(QString, argument));
    } else {
        qWarning("QtInstance::HandleMessage supports messages with zero or one arguements only");
    }
}

void QPepperInstance::flush()
{
    QtPepperMain *pepperMain = QtPepperMain::get();
    QMutexLocker lock(&pepperMain->m_mutex);
    if (pepperMain->m_exitNow)
        return;
    m_inFlush = true;
    QtModule::getCore()->CallOnMainThread(0, m_callbackFactory.NewCallback(&QPepperInstance::flushFunction), 0);
}

void QPepperInstance::waitForFlushed()
{
    QtPepperMain *pepperMain = QtPepperMain::get();
    QMutexLocker lock(&pepperMain->m_mutex);

    // The Qt thread cannot proceed until the flush has completed (it
    // is going to paint a new frame next). It will be woken up when
    // the flush has completed with m_inFlush set to false, but there
    // might also be other wakeups due to timers or input events. These
    // cannot be processed at this point.
    while (m_inFlush) {
        pepperMain->qtThreadWait_impl();
    }
}

void QPepperInstance::flushFunction(int32_t)
{
    if (!m_context2D) {
        m_inFlush = false;
        return;
    }

    m_context2D->PaintImageData(*m_imageData2D, pp::Point(),
                                 pp::Rect(0, 0, m_context2D->size().width(),
                                                m_context2D->size().height()));
    m_context2D->Flush(m_callbackFactory.NewCallback(&QPepperInstance::flushCompletedCallback));
}

void QPepperInstance::flushCompletedCallback(int32_t)
{
    QtPepperMain *pepperMain = QtPepperMain::get();
    QMutexLocker lock(&pepperMain->m_mutex);
    m_inFlush = false;

    //qDebug() << "flushCompleted" << QThread::currentThreadId();
    QTimer::singleShot(0, pepperMain, SLOT(flushCompleted()));

    if (pepperMain->m_qtWaiting)
        pepperMain->m_qtWait.wakeOne();
}

void QPepperInstance::setupTestGraphics(pp::Size newSize)
{
    QSize qtSize = toQSize(newSize);

    m_context2D = new Graphics2D(this, newSize, false);
    if (!BindGraphics(*m_context2D)) {
        printf("Couldn't bind the device context\n");
    }

    m_imageData2D = new ImageData(this, PP_IMAGEDATAFORMAT_BGRA_PREMUL, newSize, true);

    QImage qtImage = QImage(reinterpret_cast<uchar *>(m_imageData2D->data()),
                     qtSize.width(), qtSize.height(),
                     m_imageData2D->stride(),
                     QImage::Format_ARGB32_Premultiplied);

    qtImage.fill(Qt::red);
    m_context2D->PaintImageData(*m_imageData2D, pp::Point(0,0));
    //m_context2D->Flush(pp::CompletionCallback(flush_callback, 0));
}
