/*
    Licensed under the MIT license.
*/
#include "qpepperplatformwindow.h"
#include "qpepperwindowsurface.h"
#include "qpepperglcontext.h"
#include "qpeppermain.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QPepperPlatformWindow::QPepperPlatformWindow(QWidget *windowWidget)
:QPlatformWindow(windowWidget)
,m_isVisible(false)
,m_pepperGlContext(0)
{

}

QPlatformGLContext *QPepperPlatformWindow::glContext() const
{
    qDebug() << "QPepperPlatformWindow::glContext";

    if (m_pepperGlContext == 0)
        m_pepperGlContext = new QPepperGLContext(QtPepperMain::globalInstance()->instance());

    return m_pepperGlContext;
}

WId QPepperPlatformWindow::winId() const
{
    return WId(this);
}

void QPepperPlatformWindow::setVisible(bool visible)
{
    m_isVisible = visible;

    // ### repaint on visibity changes. This kind-of works for
    // the combobox case.
    widget()->update();
    if (!visible) {
        widget()->parentWidget()->update();
        widget()->parentWidget()->window()->update();
    }
}

void QPepperPlatformWindow::raise()
{

}

void  QPepperPlatformWindow::lower()
{

}

QT_END_NAMESPACE
