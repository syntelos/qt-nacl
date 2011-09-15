/*
    Licensed under the MIT license.
*/
#ifndef QPEPPERPLATFORMWINDOW_H
#define QPEPPERPLATFORMWINDOW_H

#include <QtGui/QPlatformWindow>

QT_BEGIN_NAMESPACE

class QPepperWindowSurface;
class QPepperGLContext;
class QPepperPlatformWindow : public QPlatformWindow
{
public:
    QPepperPlatformWindow(QWidget *windowWidget);
    QPlatformGLContext *glContext() const;
    WId winId() const;
    void setVisible(bool visible);
    void raise();
    void lower();
    bool m_isVisible;
private:
    mutable QPepperGLContext *m_pepperGlContext;
};

QT_END_NAMESPACE

#endif
