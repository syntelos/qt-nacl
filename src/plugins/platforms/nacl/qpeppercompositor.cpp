#include "qpeppercompositor.h"

#include <QtGui>
#include <QtGui/private/qwindowsurface_p.h>

#include "qpeppermain.h"

QPepperCompositedWindow::QPepperCompositedWindow()
    :window(0)
    ,surface(0)
    ,frameBuffer(0)
    ,parentWindow(0)
    ,flushPending(false)
    ,visible(false)
{

}

QPepperCompositor::QPepperCompositor()
    :m_pepperInstance(0)
    ,m_frameBuffer(0)
    ,m_needComposit(false)
{

}

void QPepperCompositor::addRasterWindow(QPlatformWindow *window, QPlatformWindow *parentWindow)
{
    QPepperCompositedWindow compositedWindow;
    compositedWindow.window = window;
    compositedWindow.parentWindow = parentWindow;
    m_compositedWindows.insert(window->widget(), compositedWindow);

    if (parentWindow == 0) {
        m_windowStack.append(window->widget());
    } else {
        m_compositedWindows[parentWindow->widget()].childWindows.append(window->widget());
    }
}

void QPepperCompositor::removeWindow(QPlatformWindow *window)
{
    QWidget *parentWindow = m_compositedWindows[window->widget()].parentWindow->widget();
    if (parentWindow) {
        m_compositedWindows[parentWindow].childWindows.removeAll(window->widget());
    }
    m_windowStack.removeAll(window->widget());
    m_compositedWindows.remove(window->widget());
}

void QPepperCompositor::setWindowSurface(QPlatformWindow *window, QWindowSurface *windowSurface)
{
    m_compositedWindows[window->widget()].surface = windowSurface;
}

void QPepperCompositor::setGeometry(QPlatformWindow *window, QRect geometry)
{
    m_compositedWindows[window->widget()].geometry = geometry;
}

void QPepperCompositor::setVisible(QPlatformWindow *window, bool visible)
{
    QPepperCompositedWindow &compositedWindow = m_compositedWindows[window->widget()];
    if (compositedWindow.visible == visible)
        return;

    // qDebug() << "setVisible " << this << visible;

    compositedWindow.visible = visible;
    compositedWindow.flushPending = true;
    compositedWindow.damage = compositedWindow.geometry;
    maybeComposit();
}

void QPepperCompositor::raise(QPlatformWindow *window)
{
    m_windowStack.removeAll(window->widget());
    m_windowStack.append(window->widget());
}

void QPepperCompositor::lower(QPlatformWindow *window)
{
    m_windowStack.removeAll(window->widget());
    m_windowStack.prepend(window->widget());
}

void QPepperCompositor::setFrameBuffer(QWindowSurface *surface, QImage *frameBuffer)
{
    m_compositedWindows[surface->window()].frameBuffer = frameBuffer;
}

void QPepperCompositor::flush(QWindowSurface *surface)
{
    // qDebug() << "flush" << surface->window();

    QPepperCompositedWindow &compositedWindow = m_compositedWindows[surface->window()];
    compositedWindow.flushPending = true;
    compositedWindow.damage = compositedWindow.geometry;
    maybeComposit();
}

void QPepperCompositor::waitForFlushed(QWindowSurface *surface)
{
    if (!m_compositedWindows[surface->window()].flushPending)
        return;

    QtPepperMain *pepperMain = QtPepperMain::get();
    while (m_pepperInstance->m_inFlush) {
        QMutexLocker lock(&pepperMain->m_mutex);
        pepperMain->qtThreadWait_impl();
    }
}

void QPepperCompositor::setPepperInstance(QPepperInstance *pepperInstance)
{
    m_pepperInstance = pepperInstance;
}

void QPepperCompositor::setRasterFrameBuffer(QImage *frameBuffer)
{
    m_frameBuffer = frameBuffer;
    //m_frameBuffer->fill(Qt::green);
    m_frameBuffer->fill(Qt::transparent);
}

// called from the Qt thread
void QPepperCompositor::flushCompleted()
{
    if (m_needComposit) {
        composit();
        m_needComposit = false;
    }
}

QWidget *QPepperCompositor::windowAt(QPoint p)
{
    int index = m_windowStack.count() -1;
    while (index >= 0) {
        QPepperCompositedWindow &compositedWindow
                = m_compositedWindows[m_windowStack.at(index)];
        if (compositedWindow.visible && compositedWindow.geometry.contains(p))
            return m_windowStack.at(index);
        --index;
    }
    return 0;
}

QWidget *QPepperCompositor::keyWindow()
{
    return m_windowStack.at(m_windowStack.count() - 1);
}

void QPepperCompositor::maybeComposit()
{
    if (!m_pepperInstance->m_inFlush)
        composit();
    else
        m_needComposit = true;
}

void QPepperCompositor::composit()
{
    if (!m_frameBuffer) {
        qWarning("QPepperCompositor: No frame buffer set");
        return;
    }
    if (!m_pepperInstance) {
        qWarning("QPepperCompositor: No Pepper instance set");
        return;
    }

    m_pepperInstance->waitForFlushed();

    QPainter p(m_frameBuffer);

    // ### for now, clear the entire frame.
    p.fillRect(QRect(QPoint(0,0), m_frameBuffer->size()), QBrush(Qt::transparent));

    QRegion damaged;
    foreach (QWidget *window, m_windowStack) {
        QPepperCompositedWindow &compositedWindow = m_compositedWindows[window];
     //   qDebug() << "unite" << compositedWindow.damage;
        damaged = damaged.united(compositedWindow.damage);
    //    qDebug() << "res" << damaged;
    }

    //foreach (const QRect &rect, damaged.rects()) {
    //    p.fillRect(rect, QBrush(Qt::transparent));
    //    qDebug() << "fill" << rect;
    //}

    QRegion painted;

    foreach (QWidget *window, m_windowStack) {
        QPepperCompositedWindow &compositedWindow = m_compositedWindows[window];

        if (compositedWindow.visible) {
            p.drawImage(compositedWindow.geometry.topLeft(), *compositedWindow.frameBuffer);
            painted.unite(compositedWindow.damage);
        }

        compositedWindow.flushPending = false;
        compositedWindow.damage = QRect();
    }

    //QRegion needsClear = damaged - painted;
    //qDebug() << "needsclear" << needsClear;

    m_pepperInstance->flush();
}

