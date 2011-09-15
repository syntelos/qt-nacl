/*
    Licensed under the MIT license.
*/
#include "pepperinstance.h"
#include <stdio.h>

#ifndef QT_PEPPER_STANDALONE_MODE
#include "qpeppermain.h"
#include <QWindowSystemInterface>
#endif

#include "ppapi/cpp/var.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/completion_callback.h"

using namespace pp;

QtInstance::QtInstance(PP_Instance instance)
: pp::Instance(instance)
{

}

QtInstance::~QtInstance()
{

}

bool QtInstance::Init(uint32_t argc, const char* argn[], const char* argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argn);
    Q_UNUSED(argv);
#ifndef QT_PEPPER_STANDALONE_MODE
    // Initialize the qtPepperMain global static.
    QtPepperMain::globalInstance()->init(this); // ### pass arguments?
#endif

    RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL | PP_INPUTEVENT_CLASS_KEYBOARD);

    return true;
}

void QtInstance::DidChangeView(const Rect& geometry, const Rect& clip)
{
    Q_UNUSED(clip);
#ifndef QT_PEPPER_STANDALONE_MODE
    QtPepperMain *pepperMain = QtPepperMain::globalInstance();

    if (geometry.size() == pepperMain->m_graphicsContext.size())
        return;

   // This is the entry point where we start the Qt plugin.
   // This could perhaps have been done on the PP_Module,
   // but since Qt expects having at least one screen we
   // do it on the first instance instead. (There is one
   // instance for each time you say <embed> on the web
   // page.)
   if (!pepperMain->isQtStarted()) {

       setupGraphics(geometry.size());

       // Start the main Qt thread. The thread will call
       // main() in the application code, construct the
       // QApplication object, load the pepper lighthouse
       // plugin, create the GUI and then eventually go
       // to sleep somewhere in the lighthouse event
       // dispatcher.
       pepperMain->startQtMainThread();
    } else {
       // <embed> tag resize handling:
       if (pepperMain->m_screenSize != toQSize(geometry.size()))  {

            // Park the Qt thread: Set the m_screenResized
            // flag which will cause the Qt thread to wait.
            QMutexLocker lock(&pepperMain->m_mutex);
            pepperMain->m_screenResized = true;

            // Is the qt thread aldready waiting? If not then
            // the Pepper threads needs to wait until it is.
            if (!pepperMain->m_qtWaiting) {
                pepperMain->m_pepperWait.wait(&pepperMain->m_mutex);
            }
            pepperMain->m_screenResized = false;

            // At this point the Qt thread is not running, new graphics
            // buffers and contexts can be allocated. This allocation
            // has to happen on the pepper thread.
            setupGraphics(geometry.size());

            // Post the geometry change event to lighthouse, wake the
            // Qt thread so it can process it. The Qt thread will
            // eventually end up in QPepperWindowSurface::resize
            // where it uses the new graphics buffer.
            QWindowSystemInterface::handleScreenGeometryChange(0);
            pepperMain->m_inFlush = false; // ### prevent locking up the Qt thread.
            pepperMain->m_qtWait.wakeOne();
       }
    }
#endif
}

void QtInstance::DidChangeFocus(bool has_focus)
{
    Q_UNUSED(has_focus);
}

bool QtInstance::HandleInputEvent(const pp::InputEvent& event)
{
#ifndef QT_PEPPER_STANDALONE_MODE
    if (QtPepperMain::globalInstance()->m_qtReadyForEvents == false)
        return false;
    // Translate and post event to Qt via QWindowSystemInterface.
    bool ret = eventTranslator.processEvent(event);

    // The Qt thread is most likely waiting on a timer timeout,
    // wake it so it can process the event.
    QtPepperMain *qtPepperMain = QtPepperMain::globalInstance();
    QMutexLocker lock(&qtPepperMain->m_mutex);
    qtPepperMain->m_qtWait.wakeOne();
    return ret;
#endif
    return false;
}

bool QtInstance::HandleDocumentLoad(const URLLoader& url_loader)
{
    Q_UNUSED(url_loader);
    return false;
}

// HandleMessage expects a message of the format "funcitonName:argument", or
// "functionName" and will call the corresponding slot on qtScriptableObject.
void QtInstance::HandleMessage(const Var& var_message)
{
    extern QObject *qtScriptableObject; //qglobal.cpp
    if (qtScriptableObject == 0)
        return;

    QString message = QString(var_message.AsString().data());

    QStringList parts = message.split(":");
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

void QtInstance::setupGraphics(pp::Size newSize)
{
#ifndef QT_PEPPER_STANDALONE_MODE
    QtPepperMain *pepperMain = QtPepperMain::globalInstance();
    pepperMain->m_graphicsContext = Graphics2D(this, newSize, false);
    if (!BindGraphics(pepperMain->m_graphicsContext)) {
        printf("Couldn't bind the device context\n");
    }

    pepperMain->m_imageData = ImageData(pepperMain->instance(),
                           PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                           newSize, true);
    pepperMain->m_screenSize = toQSize(newSize);
#endif
}




