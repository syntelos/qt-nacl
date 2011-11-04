#ifndef QPEPPERCOMPOSITOR_H
#define QPEPPERCOMPOSITOR_H

#include <QtGui>
#include <QtCore>
#include "pepperinstance.h"

class QPepperCompositedWindow
{
public:
    QPepperCompositedWindow();

    QPlatformWindow *window;
    QWindowSurface *surface;
    QImage *frameBuffer;
    QPlatformWindow *parentWindow;
    QRect geometry;
    QRect damage;
    bool flushPending;
    bool visible;
    QList<QWidget *> childWindows;
};

class QPepperCompositor //: public QObject
{
//Q_OBJECT
public:
    QPepperCompositor();
// Client API
    void addRasterWindow(QPlatformWindow *window, QPlatformWindow *parentWindow = 0);
    void removeWindow(QPlatformWindow *window);

    void setWindowSurface(QPlatformWindow *window, QWindowSurface *windowSurface);
    void setGeometry(QPlatformWindow *window, QRect geometry);
    void setVisible(QPlatformWindow *window, bool visible);
    void raise(QPlatformWindow *window);
    void lower(QPlatformWindow *window);

    void setFrameBuffer(QWindowSurface *surface, QImage *frameBuffer);
    void flush(QWindowSurface *surface);
    void waitForFlushed(QWindowSurface *surface);

// Server API
    void setPepperInstance(QPepperInstance *pepperInstance);
    void setRasterFrameBuffer(QImage *frameBuffer);
//public slots:
    void flushCompleted();

// Misc API
    QWidget *windowAt(QPoint p);
    QWidget *keyWindow();
    void maybeComposit();
    void composit();
private:
    QHash<QWidget *, QPepperCompositedWindow> m_compositedWindows;
    QList<QWidget *> m_windowStack;
    QPepperInstance *m_pepperInstance;
    QImage *m_frameBuffer;
    bool m_needComposit;
};

#endif
