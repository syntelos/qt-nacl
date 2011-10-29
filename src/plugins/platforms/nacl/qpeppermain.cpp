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
    QtPepperMain *qtPepperMain = QtPepperMain::get();
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

    QtPepperMain *qtPepperMain = QtPepperMain::get();
    QMutexLocker lock(&qtPepperMain->m_mutex);
    call->done = true;
    QtPepperMain::get()->m_qtWait.wakeOne();
    delete call;
}

Q_GLOBAL_STATIC(QtPepperMain, qtPepperMain);

QtPepperMain *QtPepperMain::get()
{
    return qtPepperMain();
}

QtPepperMain::QtPepperMain()
:m_mainFunction(0)
,m_mainInstance(0)
,m_qtRunning(false) // set when the qt thread is running/has been started
,m_qtReadyForEvents(false) // set when Qt is ready to handle events
,m_exitNow(false) // set when Qt should exit immediately.
,m_qtShouldPark(false) // set when the Qt thread should park (wait)
,m_qtWaiting(false) // set when the Qt thread is waiting
,m_pepperWaiting(false) // set when the screen has been resized, cleared when the Qt thread has processed the resize


{
    extern void (*qt_pepper_wait)(int);
    qt_pepper_wait = qt_pepper_wait_handler;
    extern void (*qtRunOnPepperThreadPointer)(void (*fn)(void *), void *);
    qtRunOnPepperThreadPointer = qtRunOnPepperThreadImplementation;
    m_callbackFactory.Initialize(this);
}

bool QtPepperMain::isQtStarted()
{
    return m_qtRunning;
}

void QtPepperMain::registerQtMainFunction(QtMainFunction qtMainFunction)
{
    m_mainFunction = qtMainFunction;
}

void qtThreadWait_callback(void* user_data, int32_t result)
{
    Q_UNUSED(user_data);
    Q_UNUSED(result);
    QtPepperMain *qtPepperMain = QtPepperMain::get();
    QMutexLocker lock(&qtPepperMain->m_mutex);
    qtPepperMain->m_qtWait.wakeOne();
}

void QtPepperMain::qtThreadWait(int msec)
{
    if (m_exitNow)
        return;

    QMutexLocker lock(&m_mutex);
    QtModule::getCore()->CallOnMainThread(msec, pp::CompletionCallback(&qtThreadWait_callback, 0), 0);
    qtThreadWait_impl();
}

void QtPepperMain::qtThreadWait_impl()
{
    m_qtWaiting = true;

    // Wake the pepper thread if it is waiting.
    if (m_pepperWaiting) {
        m_pepperWait.wakeOne();
    }
    m_qtWait.wait(&m_mutex);
    m_qtWaiting = false;
}

void QtPepperMain::qtThreadWake()
{
    QMutexLocker lock(&m_mutex);
    m_qtWait.wakeOne();
}

// This function is called by the pepper thread to make sure
// the Qt thread is in a "parked" state, waiting on m_qtWait.
// The Qt thread has two ways of entering this state:
//   - Waiting for events/timers in the event loop.
//   - Blocking in QPepperWindowSurface::beginPaint, waiting for a
//     previous flush to complete.
// In both cases the Qt thread sets m_qtWaiting.
//
// This function signals that the Qt thread should park itself,
// and then waits for it to do so. In many cases there will be
// no wait, since the Qt thread is in the parked state when idling.
void QtPepperMain::parkQtThread()
{
    QMutexLocker lock(&m_mutex);
    // Is the Qt thread already parked? If so this all is good
    // and we can return immediately.
    if (m_qtWaiting)
        return;

    // Wait for the Qt thread to park. Setting m_pepperWaiting signals
    // that the Qt thread should wake the pepper thread.
    m_pepperWaiting = true;
    m_pepperWait.wait(&m_mutex);
    m_pepperWaiting = false;
}

void QtPepperMain::resumeQtThread()
{
    m_qtWait.wakeOne();
}

void QtPepperMain::postJavascriptMessage(const QByteArray &message)
{
    QtModule::getCore()->CallOnMainThread(0,
        m_callbackFactory.NewCallback(&QtPepperMain::postJavascriptMessage_impl, message), 0);
}

void QtPepperMain::postJavascriptMessage_impl(int32_t, const QByteArray &message)
{
    QPepperInstance *instance = QtPepperMain::get()->m_mainInstance;
    if (m_mainInstance) {
        m_mainInstance->PostMessage(pp::Var(message.constData()));
    }
}

void MainQtThread::run()
{
    qt_pepper_main_thread_function(0);
}

void QtPepperMain::startQtMainThread()
{
    m_qtRunning = true;

//    qDebug() << "Starting the main Qt thread";
//    qDebug() << "From thread" << QString::number((quintptr) QThread::currentThread(), 16);

    pthread_create(&m_qtMainThread, 0, qt_pepper_main_thread_function, 0);
    //m_mainQtThread.start();
    //this->moveToThread(&m_mainQtThread);
}

void *qt_pepper_main_thread_function(void *)
{
    QtPepperMain::get()->qtMainThreadFunction();
    return 0;
}

void QtPepperMain::qtMainThreadFunction()
{
   // qInstallMsgHandler(qtPepperMessageHandler);

    int argc = 3;
    const char * argv[] = { "", "-platform", "pepper" };
    qt_pepper_main(argc, const_cast<char **>(argv));

    m_qtRunning = false;
    m_pepperWait.wakeOne();
}

void QtPepperMain::runOnPepperThread(void (*fn)(void *), void *data)
{
    qtRunOnPepperThreadImplementation(fn, data);
}

void QtPepperMain::runOnPepperThreadSynchronous(void (*fn)(void *), void *data)
{
    qtRunOnPepperThreadSynchronousImplementation(fn, data);
}

void QtPepperMain::setInstance(QPepperInstance *instance)
{
    m_mainInstance = instance;
    extern pp::Instance *qtPepperInstance; // qglobal.cpp
    qtPepperInstance = m_mainInstance;
    m_compositor.setPepperInstance(instance);
}

QPepperInstance *QtPepperMain::instance()
{
    return m_mainInstance;
}


void QtPepperMain::qtShutDown()
{
}

void QtPepperMain::flushCompleted()
{
    m_compositor.flushCompleted();
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

void qt_pepper_wait_handler(int msec)
{
    QtPepperMain::get()->qtThreadWait(msec);
}
