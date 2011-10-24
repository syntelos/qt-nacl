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

/*
    There is a one-to-one correspondence between QPepperPlatformWindow,
    QPepperInstance and <embed> elements on the web page. Platform Windows
    are created from the Qt side, while pepper instances are created
    from the Pepper side.

    The connection between them is made in two ways:
    1) The first window and instance are connected as soon as
       both have been created. The first instance is created
       by the initial <embed> tag on the page, the first window
       is simply the first Qt window created. The window geometry
       tracks the <embed> tag geometry.

    2) Subsequent windows add <embed> tags to the web page, which
       causes pepper to create instances that can be connected
       to the window. Window (and instance) creation is controlled
       by Qt. Adding more <embed> tags on the web page manually is
       not supported. The <embed> tag geometry tracks the window
       geometry.
*/

QPepperPlatformWindow::QPepperPlatformWindow(QWidget *windowWidget, bool isFirstWindow)
:QPlatformWindow(windowWidget)
,m_isVisible(false)
,m_pepperGlContext(0)
{
    m_windowId = isFirstWindow ? 0 : quint32(this);
    qDebug() << "QPepperPlatformWindow::QPepperPlatformWindow" << m_windowId << isFirstWindow;

    postJavascriptMessage(QByteArray("qt_create_window"));
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
    m_isVisible = visible;
    if (visible)
        postJavascriptMessage(QByteArray("qt_show_window"));
    else
        postJavascriptMessage(QByteArray("qt_hide_window"));
}

void QPepperPlatformWindow::raise()
{
    postJavascriptMessage(QByteArray("qt_raise_window"));
}

void QPepperPlatformWindow::lower()
{
    postJavascriptMessage(QByteArray("qt_lower_window"));
}

void QPepperPlatformWindow::setGeometry(const QRect &rect)
{
    QByteArray message("qt_set_window_geometry_");
    message.append("_x_" + QByteArray::number(rect.x()));
    message.append("_y_" + QByteArray::number(rect.y()));
    message.append("_width_" + QByteArray::number(rect.width()));
    message.append("_height_" + QByteArray::number(rect.height()));
    postJavascriptMessage(message);
}

quint32 QPepperPlatformWindow::windowId()
{
    return m_windowId;
}

void QPepperPlatformWindow::postJavascriptMessage(const QByteArray &message)
{
    if (m_windowId == 0)
        return;

    QByteArray msg = message;
    msg.append("_");
    msg.append(QByteArray::number(m_windowId));
    QtPepperMain::get()->postJavascriptMessage(msg);
}

QT_END_NAMESPACE
