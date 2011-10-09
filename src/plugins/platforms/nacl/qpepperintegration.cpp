#include "qpepperintegration.h"
#include "qpepperwindowsurface.h"
#include "qpepperscreen.h"
#include "qpepperplatformwindow.h"
#include "qpeppermain.h"
#include "qpepperfontdatabase.h"


#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>
#include <qbasicunixfontdatabase.h>
#include <qdebug.h>

QPepperIntegration * QPepperIntegration::createPepperIntegration()
{
    return new QPepperIntegration();
}

QPepperIntegration::QPepperIntegration()
    : m_firstWindowCreated(false)
{

    //qDebug() << "QPepperIntegration::QPepperIntegration()";
    QPepperScreen *mPrimaryScreen = new QPepperScreen();
    m_screens.append(mPrimaryScreen);
    m_pepperScreen = mPrimaryScreen;
    m_fontDatabase = 0;
    //qDebug() << "QPepperIntegration::QPepperIntegration done()";

    QtPepperMain::globalInstance()->m_qtReadyForEvents = true;
}

QPepperIntegration::~QPepperIntegration()
{
    delete m_fontDatabase;
}

bool QPepperIntegration::hasOpenGL() const
{
    qDebug() << "PepperIntegration::hasOpenGL";
    return true;
}

QPixmapData *QPepperIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QPlatformWindow *QPepperIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    //qDebug() << "QPepperIntegration::createPlatformWindow" << widget << widget->parent()
    //         << widget->objectName() << widget->geometry();

    // Hack: The first window becomes a "fullscreen" window - meaning
    // it will expand to fit the entire <embed> area.
    if (!m_firstWindowCreated) {
        m_firstWindowCreated = true;
        widget->setWindowState(Qt::WindowFullScreen);
    }

    QPlatformWindow *platformWindow = new QPepperPlatformWindow(widget);
    m_platformWindows.insert(widget, platformWindow);

    return platformWindow;
}

QWindowSurface *QPepperIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
   // qDebug() << "QPepperIntegration::createWindowSurface" << widget << widget->parentWidget();

    // Window surfaces know about any "parent" window surfaces (for compositing)
    QPepperWindowSurface *parentWindowSurface = 0;
    if (widget->parentWidget() != 0) {
        parentWindowSurface =
            static_cast<QPepperWindowSurface*>(m_windowSurfaces.value(widget->parentWidget()->window()));
    }
    QPepperWindowSurface *windowSurface = new QPepperWindowSurface(widget, parentWindowSurface);
    m_windowSurfaces.insert(widget, windowSurface);
    return windowSurface;
}

QList<QPlatformScreen *> QPepperIntegration::screens() const
{
    return m_screens;
}

QPlatformFontDatabase *QPepperIntegration::fontDatabase() const
{
    if (m_fontDatabase == 0)
        m_fontDatabase = new QPepperFontDatabase();

    return m_fontDatabase;
}
