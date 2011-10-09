/*
    Licensed under the MIT license.
*/
#ifndef QPLATFORMINTEGRATION_PEPPER_H
#define QPLATFORMINTEGRATION_PEPPER_H

#include <QtGui/QPlatformIntegration>
#include <QtGui/QPlatformScreen>
#include "qpepperscreen.h"


QT_BEGIN_NAMESPACE

class QPlatformFontDatabase; 
class QPepperFontDatabase;
class QPepperIntegration : public QPlatformIntegration
{
public:
    static QPepperIntegration *createPepperIntegration();
    QPepperIntegration();
    ~QPepperIntegration();
    virtual bool hasOpenGL() const;

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;

    QList<QPlatformScreen *> screens() const;

    QPlatformFontDatabase *fontDatabase() const;
private:
    QList<QPlatformScreen *> m_screens;
    QPepperScreen *m_pepperScreen;
    mutable bool m_firstWindowCreated;
    mutable QPepperFontDatabase *m_fontDatabase;
    mutable QHash<QWidget *, QWindowSurface *>m_windowSurfaces;
    mutable QHash<QWidget *, QPlatformWindow *>m_platformWindows;
};

QT_END_NAMESPACE

#endif
