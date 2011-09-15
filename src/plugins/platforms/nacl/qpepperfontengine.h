#ifndef QFONTENGINE_NACL_P_H
#define QFONTENGINE_NACL_P_H

#include <private/qfontengine_p.h>

#ifdef QT_PEPPER_USE_PEPPER_FONT_ENGINE

#include <ppapi/cpp/dev/font_dev.h>

class QFontEnginePepper : public QFontEngine
{
public:
    QFontEnginePepper(const QFontDef &fontDef);
    bool stringToCMap(const QChar*, int, QGlyphLayout*, int*, QFlags<QTextEngine::ShaperFlag>) const;
    glyph_metrics_t boundingBox(const QGlyphLayout&);
    glyph_metrics_t boundingBox(glyph_t);
    QFixed ascent() const;
    QFixed descent() const;
    QFixed leading() const;
    qreal maxCharWidth() const;
    const char* name() const;
    bool canRender(const QChar*, int);
    QFontEngine::Type type() const;
private:
    QString m_fontName;
    QFontDef m_fontDef;
    pp::FontDescription_Dev m_pepperRequestedFontDescription;
    pp::FontDescription_Dev m_pepperActualFontDescription;
    pp::Font_Dev m_pepperFont;
    PP_FontMetrics_Dev m_pepperFontMetrics;
};

#endif
#endif
