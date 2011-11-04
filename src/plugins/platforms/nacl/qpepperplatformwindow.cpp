/*
    Licensed under the MIT license.
*/
#include "qpepperplatformwindow.h"
#include "qpepperwindowsurface.h"
#include "qpepperglcontext.h"
#include "qpeppermain.h"
#include "peppermodule.h"
#include "qpeppercompositor.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QPepperPlatformWindow::QPepperPlatformWindow(QWidget *windowWidget, bool isFirstWindow)
:QPlatformWindow(windowWidget)
,m_isVisible(false)
,m_pepperGlContext(0)
,m_compositor(&QtPepperMain::get()->m_compositor)
{
    m_windowId = isFirstWindow ? 0 : quint32(this);
    m_compositor->addRasterWindow(this);
    // qDebug() << "QPepperPlatformWindow::QPepperPlatformWindow" << m_windowId << isFirstWindow;
}

QPepperPlatformWindow::~QPepperPlatformWindow()
{
    m_compositor->removeWindow(this);
}

QPlatformGLContext *QPepperPlatformWindow::glContext() const
{
    // qDebug() << "QPepperPlatformWindow::glContext";

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
    m_compositor->setVisible(this, visible);
}

void QPepperPlatformWindow::raise()
{
    m_compositor->raise(this);
}

void QPepperPlatformWindow::lower()
{
    m_compositor->lower(this);
}

void QPepperPlatformWindow::setGeometry(const QRect &rect)
{
    // qDebug() << "setGeometry" << rect;
    m_compositor->setGeometry(this, rect);
}

quint32 QPepperPlatformWindow::windowId()
{
    return m_windowId;
}

QT_END_NAMESPACE
