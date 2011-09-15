/*
    Licensed under the MIT license.
*/
#ifndef QPEPPERSCREEN_H
#define QPEPPERSCREEN_H

#include <QtGui/QPlatformScreen>

QT_BEGIN_NAMESPACE

class QPepperScreen : public QPlatformScreen
{
public:
    QPepperScreen();
    QRect geometry() const;
    int depth() const { return m_depth; }
    QImage::Format format() const { return m_format; }

public:
    int m_depth;
    QImage::Format m_format;
};

QT_END_NAMESPACE

#endif
