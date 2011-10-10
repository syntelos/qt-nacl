/*
    Licensed under the MIT license.
*/
#include "qpeppermain.h"
#include "peppermodule.h"
#include <ppapi/cpp/completion_callback.h>
#include <qdebug.h>
#include <qthread.h>
#include <qfile.h>

int qt_pepper_main(int argc, char**argv);
void *qt_pepper_main_thread_function(void *);
void qt_pepper_wait_handler(int msec);

Q_IMPORT_PLUGIN(pepper);

void qtRunOnPepperThreadCallback(void* user_data, int32_t result);
struct RunOnPepperThreadCall
{
    void (*fn)(void *);
    void* data;
    bool wakeQtThread;
    bool done;
};

void qtRunOnPepperThreadImplementation(void (*fn)(void *), void *data)
{
    RunOnPepperThreadCall *call = new RunOnPepperThreadCall;
    call->fn = fn;
    call->data = data;
    call->wakeQtThread = false;
 //   qDebug() << "qtRunOnPepperThreadImplementation" << fn << QtModule::getCore()->IsMainThread();

    QtModule::getCore()->CallOnMainThread(0, pp::CompletionCallback(&qtRunOnPepperThreadCallback, call), 0);
}

void qtRunOnPepperThreadSynchronousImplementation(void (*fn)(void *), void *data)
{
    RunOnPepperThreadCall *call = new RunOnPepperThreadCall;
    call->fn = fn;
    call->data = data;
    call->wakeQtThread = true;
    call->done = false;
    QtModule::getCore()->CallOnMainThread(0, pp::CompletionCallback(&qtRunOnPepperThreadCallback, call), 0);
    QtPepperMain *qtPepperMain = QtPepperMain::globalInstance();
    QMutexLocker lock(&qtPepperMain->m_mutex);
    if (call->done)
        return;

    qtPepperMain->m_qtWait.wait(&qtPepperMain->m_mutex);
}

void qtRunOnPepperThreadCallback(void* user_data, int32_t result)
{
    Q_UNUSED(result);
    RunOnPepperThreadCall *call = reinterpret_cast<RunOnPepperThreadCall *>(user_data);
    call->fn(call->data);
    if (!call->wakeQtThread) {
        delete call;
        return;
    }

    QtPepperMain *qtPepperMain = QtPepperMain::globalInstance();
    QMutexLocker lock(&qtPepperMain->m_mutex);
    call->done = true;
    QtPepperMain::globalInstance()->m_qtWait.wakeOne();
    delete call;
}

Q_GLOBAL_STATIC(QtPepperMain, qtPepperMain);

QtPepperMain *QtPepperMain::globalInstance()
{
    return qtPepperMain();
}

QtPepperMain::QtPepperMain()
:m_mainFunction(0)
,m_mainInstance(0)
,m_qtRunning(false) // set when the qt thread is running/has been started
,m_qtReadyForEvents(false) // set when Qt is ready to handle events
,m_exitNow(false) // set when Qt should exit immediately.
,m_screenResized(false) // set when the screen has been resized, cleared when the Qt thread has processed the resize
,m_inFlush(false) // set when the pepper thread is flushing the graphichs buffer
,m_qtWaiting(false) // set when the Qt thread is waiting

{
    extern void (*qt_pepper_wait)(int);
    qt_pepper_wait = qt_pepper_wait_handler;
    extern void (*qtRunOnPepperThreadPointer)(void (*fn)(void *), void *);
    qtRunOnPepperThreadPointer = qtRunOnPepperThreadImplementation;
}

bool QtPepperMain::isQtStarted()
{
    return m_qtRunning;
}

void QtPepperMain::init(QtInstance *mainInstance)
{
    m_mainInstance = mainInstance;

    extern pp::Instance *qtPepperInstance; // qglobal.cpp
    qtPepperInstance = mainInstance;
}

void QtPepperMain::registerQtMainFunction(QtMainFunction qtMainFunction)
{
    m_mainFunction = qtMainFunction;
}

void QtPepperMain::startQtMainThread()
{
    m_qtRunning = true;

//    qDebug() << "Starting the main Qt thread";
//    qDebug() << "From thread" << QString::number((quintptr) QThread::currentThread(), 16);

    pthread_create(&m_qtMainThread, 0, qt_pepper_main_thread_function, this);
}

void QtPepperMain::runOnPepperThread(void (*fn)(void *), void *data)
{
    qtRunOnPepperThreadImplementation(fn, data);
}

void QtPepperMain::runOnPepperThreadSynchronous(void (*fn)(void *), void *data)
{
    qtRunOnPepperThreadSynchronousImplementation(fn, data);
}

#ifndef QT_NO_PEPPER_INTEGRATION

QtInstance *QtPepperMain::instance()
{
    return m_mainInstance;
}

#ifndef QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION
pp::ImageData QtPepperMain::imageData()
{
    return m_imageData;
}
#endif
#endif

void QtPepperMain::qtShutDown()
{
    m_qtRunning = false;
    m_pepperWait.wakeOne();
}

void qtPepperMessageHandlerPrinter(void *msg)
{
    const char * test = reinterpret_cast<const char *>(msg);
    printf("%s\n", test);
    free(msg);
}

void qtPepperMessageHandler(QtMsgType typ, const char *msg)
{

    qtRunOnPepperThreadImplementation(qtPepperMessageHandlerPrinter, const_cast<char *>(qstrdup(msg)));
}

void *qt_pepper_main_thread_function(void *data)
{
    Q_UNUSED(data);
    // qDebug() << "QtPepperMainThread::run entered" <<QString::number((quintptr) QThread::currentThread(), 16);
    QtPepperMain *qtPepperMain = QtPepperMain::globalInstance();

   // qInstallMsgHandler(qtPepperMessageHandler);

    int argc = 3;
    const char * argv[] = { "", "-platform", "pepper" };
    qt_pepper_main(argc, const_cast<char **>(argv));

    qtPepperMain->qtShutDown(); // ### may be to early to call this.. what about global destructors?

    return 0;
}

void qt_pepper_wakeup(void* user_data, int32_t result)
{
    Q_UNUSED(user_data);
    Q_UNUSED(result);
    QtPepperMain *qtPepperMain = QtPepperMain::globalInstance();
    QMutexLocker lock(&qtPepperMain->m_mutex);
    qtPepperMain->m_qtWait.wakeOne();
}

void qt_pepper_wait_handler(int msec)
{
    QtPepperMain *qtPepperMain = QtPepperMain::globalInstance();
    if (qtPepperMain->m_exitNow)
        return;

    QMutexLocker lock(&qtPepperMain->m_mutex);
    QtModule::getCore()->CallOnMainThread(msec, pp::CompletionCallback(&qt_pepper_wakeup, 0), 0);
    qtPepperMain->m_qtWaiting = true;

    // If there is a pending resize the pepper thread might be waiting
    // for the Qt thread to sleep. Wake it.
    if (qtPepperMain->m_screenResized) {
        qtPepperMain->m_pepperWait.wakeOne();
    }
    qtPepperMain->m_qtWait.wait(&qtPepperMain->m_mutex);
    qtPepperMain->m_qtWaiting = false;
}

#ifndef QT_NO_PEPPER_INTEGRATION
QSize toQSize(pp::Size size) { return QSize(size.width(), size.height()); }
pp::Size toPPSize(const QSize &size) { return pp::Size(size.width(), size.height()); }
#endif
