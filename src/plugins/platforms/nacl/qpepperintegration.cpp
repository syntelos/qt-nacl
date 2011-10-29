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

    QtPepperMain::get()->m_qtReadyForEvents = true;
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
    qDebug() << "QPepperIntegration::createPlatformWindow" << widget << widget->parent()
             << widget->objectName() << widget->geometry() << m_firstWindowCreated;

    QPepperPlatformWindow *platformWindow = new QPepperPlatformWindow(widget, !m_firstWindowCreated);
    platformWindow->m_trackInstanceSize = true;
    if (!m_firstWindowCreated)
        widget->setWindowState(Qt::WindowFullScreen);
    m_firstWindowCreated = true;
    return platformWindow;
}

QWindowSurface *QPepperIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
   // qDebug() << "QPepperIntegration::createWindowSurface" << widget << widget->parentWidget();

    QPepperWindowSurface *windowSurface = new QPepperWindowSurface(widget);
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
