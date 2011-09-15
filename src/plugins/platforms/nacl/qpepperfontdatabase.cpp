#include "qpepperfontdatabase.h"
#include <QtGui>
#ifdef QT_PEPPER_USE_PEPPER_FONT_ENGINE
#include <QtGui/private/qfontengine_nacl_p.h>
#endif

void QPepperFontDatabase::populateFontDatabase()
{
#ifdef QT_PEPPER_USE_PEPPER_FONT_ENGINE
    QSupportedWritingSystems writingSystems;
    writingSystems.setSupported(QFontDatabase::Latin);
    registerFont("Arial", "",
                 QFont::Normal,
                 QFont::StyleNormal,
                 QFont::Unstretched,
                 true,
                 true,
                 12,
                 writingSystems,
                 0);
#else
    // Load font file from resources. Currently
    // all fonts needs to be bundled with the nexe
    // as Qt resources.
    QByteArray fontFileName = ":/fonts/Vera.ttf";
    QFile theFont(fontFileName);
    theFont.open(QIODevice::ReadOnly);
    QBasicUnixFontDatabase::addTTFile(theFont.readAll(), fontFileName);
#endif
}

QFontEngine *QPepperFontDatabase::fontEngine(const QFontDef &fontDef, QUnicodeTables::Script script, void *handle)
{
   // qDebug() << "QPepperFontDatabase::fontEngine" << fontDef.family << fontDef.pixelSize << fontDef.pointSize;
#ifdef QT_PEPPER_USE_PEPPER_FONT_ENGINE
    return new QFontEnginePepper(fontDef);
#else
    return QBasicUnixFontDatabase::fontEngine(fontDef, script, handle);
#endif
}

QStringList QPepperFontDatabase::fallbacksForFamily(const QString family, const QFont::Style &style, const QFont::StyleHint &styleHint, const QUnicodeTables::Script &script) const
{
    // qDebug() << "QPepperFontDatabase::fallbacksForFamily" << family;
    QStringList fallbacks = QBasicUnixFontDatabase::fallbacksForFamily(family, style, script);
    // qDebug() << "Fallbacks" << fallbacks;
#ifdef QT_PEPPER_USE_PEPPER_FONT_ENGINE
    if (fallbacks.isEmpty())
        fallbacks.append("Arial");
#endif
    return fallbacks;
}

QStringList QPepperFontDatabase::addApplicationFont(const QByteArray &fontData, const QString &fileName)
{
    // qDebug() << "QPepperFontDatabase::addApplicationFont";
    return QBasicUnixFontDatabase::addApplicationFont(fontData, fileName);
}

void QPepperFontDatabase::releaseHandle(void *handle)
{
    // qDebug() << "QPepperFontDatabase::releaseHandle" << handle;
    QBasicUnixFontDatabase::releaseHandle(handle);
}
