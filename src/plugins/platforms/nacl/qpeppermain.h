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
#include <qdebug.h>

typedef void *QtMainFunction; // ### fixify

class QtPepperMain : public QObject
{
Q_OBJECT
public:
    static QtPepperMain *globalInstance();
    QtPepperMain();
    bool isQtStarted();
    void init(QtInstance *mainInstance);
    void registerQtMainFunction(QtMainFunction mainFunction);
    void startQtMainThread();

    void runOnPepperThread(void (*fn)(void *), void *data);
    void runOnPepperThreadSynchronous(void (*fn)(void *), void *data);

    void log(const QString &message);
#ifndef QT_NO_PEPPER_INTEGRATION
    QtInstance *instance();
    pp::ImageData imageData();
#endif
public Q_SLOTS:
    void qtShutDown();
public:
    QtMainFunction m_mainFunction;
    pthread_t m_qtMainThread;
    QtInstance *m_mainInstance;
    bool m_qtRunning;
    bool m_qtReadyForEvents;
    bool m_exitNow;

    QSize m_screenSize;

    pp::Graphics2D m_graphicsContext;
    pp::ImageData m_imageData;

    bool m_screenResized;

    QMutex m_mutex;
    bool m_inFlush;
    bool m_qtWaiting;
    QWaitCondition m_qtWait;
    QWaitCondition m_pepperWait;
};

#ifndef QT_NO_PEPPER_INTEGRATION
QSize toQSize(pp::Size size);
pp::Size toPPSize(const QSize &size);
#endif

#endif // QPEPPER_MAIN_H
