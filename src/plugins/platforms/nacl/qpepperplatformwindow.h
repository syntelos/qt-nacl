/*
    Licensed under the MIT license.
*/
#ifndef QPEPPERPLATFORMWINDOW_H
#define QPEPPERPLATFORMWINDOW_H

#include <QtGui/QPlatformWindow>

#include "qpepperhelpers.h"
#include "qpeppercompositor.h"


QT_BEGIN_NAMESPACE

class QPepperWindowSurface;
class QPepperGLContext;
class QPepperPlatformWindow : public QPlatformWindow
{
public:
    QPepperPlatformWindow(QWidget *windowWidget, bool isFirstWindow);
    ~QPepperPlatformWindow();
    QPlatformGLContext *glContext() const;
    WId winId() const;
    void setVisible(bool visible);
    void raise();
    void lower();
    void setGeometry(const QRect &rect);
    quint32 windowId();

    bool m_isVisible;
    bool m_trackInstanceSize;
    quint32 m_windowId;
private:
    mutable QPepperGLContext *m_pepperGlContext;
    QPepperCompositor *m_compositor;
};

QT_END_NAMESPACE

#endif
