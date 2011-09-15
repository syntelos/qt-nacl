/*
    Licensed under the MIT license.
*/
#ifndef QPEPPERFONTDATABASE_H
#define QPEPPERFONTDATABASE

#include <qbasicunixfontdatabase.h>

class QPepperFontDatabase : public QBasicUnixFontDatabase
{
public:
    void populateFontDatabase();
    QFontEngine *fontEngine(const QFontDef &fontDef, QUnicodeTables::Script script, void *handle);
    QStringList fallbacksForFamily(const QString family, const QFont::Style &style, const QFont::StyleHint &styleHint, const QUnicodeTables::Script &script) const;
    QStringList addApplicationFont(const QByteArray &fontData, const QString &fileName);
    void releaseHandle(void *handle);
};

#endif
