/*
    Licensed under the MIT license.
*/
#ifndef QWINDOWSURFACE_MINIMAL_H
#define QWINDOWSURFACE_MINIMAL_H

#include <qobject.h>
#include <QtGui/private/qwindowsurface_p.h>
#include <QtGui/QPlatformWindow>

#ifndef QT_NO_PEPPER_INTEGRATION
#include <ppapi/cpp/graphics_2d.h>
#include <ppapi/cpp/image_data.h>
#include <ppapi/cpp/rect.h>
#endif

#include "qpepperscreen.h"
#include "qpepperplatformwindow.h"
#include "pepperinstance.h"
#include "qpeppercompositor.h"

QT_BEGIN_NAMESPACE

class QPepperPlatformWindow;
class QPepperWindowSurface : public QObject, public QWindowSurface
{
Q_OBJECT
public:
    QPepperWindowSurface(QWidget *window);
    ~QPepperWindowSurface();

    void createFrameBuffer(QSize size);
    void setPepperInstance(QPepperInstance *instance);
    void setFrameBuffer(QImage *frameBuffer);

    // virtuals
    QPaintDevice *paintDevice();
    void beginPaint(const QRegion &);
    void endPaint(const QRegion &);
    bool scroll(const QRegion &area, int dx, int dy);
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void resize(const QSize &size);

    bool m_isInPaint;

protected:
    pp::Rect pepperRect(const QRect rect);
private:
    QPepperPlatformWindow *m_platformWindow;
    QPepperInstance *m_pepperInstance;
    QPepperCompositor *m_compositor;

    QImage *m_frameBuffer;
    bool m_ownsFrameBuffer;

//    friend void createPepperGraphicsContext_callback(void *context);
//    friend void flushFunction(void* context);
};

QT_END_NAMESPACE

#endif
