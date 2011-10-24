/*
    Licensed under the MIT license.
*/
#include "qpepperscreen.h"
#include "qpeppermain.h"

QT_BEGIN_NAMESPACE

/*
    A pepper screen corresponds to the browser viewport.
*/

QPepperScreen::QPepperScreen()
: m_depth(32), m_format(QImage::Format_ARGB32_Premultiplied)
{
}

QRect QPepperScreen::geometry() const
{
    // ### get viewport size
    return QRect(QPoint(0, 0), QSize(1280, 1024));
}

QT_END_NAMESPACE
