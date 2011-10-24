#ifndef QPEPPER_MAIN_H
#define QPEPPER_MAIN_H

/*
    Licensed under the MIT license.
*/
#include <qglobal.h>
#include <pthread.h>

#ifndef QT_NO_PEPPER_INTEGRATION
#include <ppapi/cpp/graphics_2d.h>
#include <ppapi/cpp/image_data.h>
#include <pepperinstance.h>
#else
typedef void * QtInstance;
#endif


#include <qpepperintegration.h>
#include <qpepperhelpers.h>
#include <qdebug.h>

typedef void *QtMainFunction; // ### fixify

class QPepperWindowSurface;
class QtPepperMain : public QObject
{
Q_OBJECT
public:
    static QtPepperMain *get();
    QtPepperMain();
    bool isQtStarted();
    void registerQtMainFunction(QtMainFunction mainFunction);
    void startQtMainThread();
    void qtMainThreadFunction();

    void runOnPepperThread(void (*fn)(void *), void *data);
    void runOnPepperThreadSynchronous(void (*fn)(void *), void *data);

    void addInstance(int key, QPepperInstance *instance);
    void removeInstance(int key);
    void addWindowSurface(int key, QPepperWindowSurface *surface);
    void removeWindowSurface(int key);

    void qtThreadWait(int msec);
    void qtThreadWait_impl();
    void qtThreadWake();
    void parkQtThread();
    void resumeQtThread();
//    void pepperThreadWait();
//    void pepperThreadWake();

    void postJavascriptMessage(const QByteArray &message);
    void postJavascriptMessage_impl(int32_t, const QByteArray &message);

    void log(const QString &message);
    QPepperInstance *instance();
//public Q_SLOTS:
//    void qtShutDown();
public:
    QtMainFunction m_mainFunction;
    pthread_t m_qtMainThread;

    QPepperInstance *m_mainInstance;
    QPepperWindowSurface *m_mainWindowSurface;
    QHash<int, QPepperInstance *> m_pepperInstances;
    QHash<int, QPepperWindowSurface *> m_windowSurfaces;

    bool m_qtRunning;
    bool m_qtReadyForEvents;
    bool m_exitNow;

    bool m_qtShouldPark;
    bool m_qtWaiting;
    QWaitCondition m_qtWait;
    bool m_pepperWaiting;
    QWaitCondition m_pepperWait;

    QMutex m_mutex;
    pp::CompletionCallbackFactory<QtPepperMain, ThreadSafeRefCount> m_callbackFactory;
};

#endif // QPEPPER_MAIN_H
