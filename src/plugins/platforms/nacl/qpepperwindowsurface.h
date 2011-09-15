/*
    Licensed under the MIT license.
*/
#ifndef QWINDOWSURFACE_MINIMAL_H
#define QWINDOWSURFACE_MINIMAL_H

#include <QtGui/private/qwindowsurface_p.h>
#include <QtGui/QPlatformWindow>

#ifndef QT_NO_PEPPER_INTEGRATION
#include <ppapi/cpp/graphics_2d.h>
#include <ppapi/cpp/image_data.h>
#include <ppapi/cpp/rect.h>
#endif

#include "qpepperscreen.h"

QT_BEGIN_NAMESPACE

class QPepperPlatformWindow;
class QPepperWindowSurface : public QWindowSurface
{
public:
    QPepperWindowSurface(QWidget *window, QPepperWindowSurface *parentWindow = 0);
    ~QPepperWindowSurface();

#ifdef QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION
    void createPepperGraphicsContext();
#endif
    void createQImagePaintDevice(QSize size);

    // virtuals
    QPaintDevice *paintDevice();
    void beginPaint(const QRegion &);
    void endPaint(const QRegion &);
    bool scroll(const QRegion &area, int dx, int dy);
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void resize(const QSize &size);

protected:
    pp::Rect pepperRect(const QRect rect);
private:
    QImage m_qtImage;
    volatile bool m_graphicsContextCreated;
#ifdef QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION
    pp::Graphics2D m_graphicsContext;
    pp::ImageData m_imageData;
#endif

    QPoint m_childWindowPos;
    QPepperWindowSurface *m_parentWindowSurface;
    QList<QPepperWindowSurface *>m_childWindowSurfaces;
    QPepperPlatformWindow *m_platformWindow;

    friend void createPepperGraphicsContext_callback(void *context);
    friend void flushFunction(void* context);
};

QT_END_NAMESPACE

#endif
