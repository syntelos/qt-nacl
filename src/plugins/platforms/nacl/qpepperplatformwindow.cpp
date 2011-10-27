/*
    Licensed under the MIT license.
*/
#include "qpepperplatformwindow.h"
#include "qpepperwindowsurface.h"
#include "qpepperglcontext.h"
#include "qpeppermain.h"
#include "peppermodule.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QPepperPlatformWindow::QPepperPlatformWindow(QWidget *windowWidget, bool isFirstWindow)
:QPlatformWindow(windowWidget)
,m_isVisible(false)
,m_pepperGlContext(0)
{
    m_windowId = isFirstWindow ? 0 : quint32(this);
    qDebug() << "QPepperPlatformWindow::QPepperPlatformWindow" << m_windowId << isFirstWindow;
}

QPlatformGLContext *QPepperPlatformWindow::glContext() const
{
    qDebug() << "QPepperPlatformWindow::glContext";

    if (m_pepperGlContext == 0)
        m_pepperGlContext = new QPepperGLContext(QtPepperMain::get()->instance());

    return m_pepperGlContext;
}

WId QPepperPlatformWindow::winId() const
{
    return WId(this);
}

void QPepperPlatformWindow::setVisible(bool visible)
{
}

void QPepperPlatformWindow::raise()
{
}

void QPepperPlatformWindow::lower()
{
}

void QPepperPlatformWindow::setGeometry(const QRect &rect)
{
}

quint32 QPepperPlatformWindow::windowId()
{
    return m_windowId;
}

QT_END_NAMESPACE
