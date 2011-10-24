/*
    Licensed under the MIT license.
*/
#ifndef QPEPPERPLATFORMWINDOW_H
#define QPEPPERPLATFORMWINDOW_H

#include <QtGui/QPlatformWindow>
#include "qpepperhelpers.h"
#include <ppapi/cpp/completion_callback.h>

QT_BEGIN_NAMESPACE

class QPepperWindowSurface;
class QPepperGLContext;
class QPepperPlatformWindow : public QPlatformWindow
{
public:
    QPepperPlatformWindow(QWidget *windowWidget, bool isFirstWindow);
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
    void postJavascriptMessage(const QByteArray &message);
    mutable QPepperGLContext *m_pepperGlContext;

};

QT_END_NAMESPACE

#endif
