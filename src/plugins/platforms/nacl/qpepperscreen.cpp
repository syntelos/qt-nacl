/*
    Licensed under the MIT license.
*/
#include "qpepperscreen.h"
#include "qpeppermain.h"

QT_BEGIN_NAMESPACE

/*
    A pepper screen corresponds to an nacl <embed> tag.
*/

QPepperScreen::QPepperScreen()
: m_depth(32), m_format(QImage::Format_ARGB32_Premultiplied)
{
}

QRect QPepperScreen::geometry() const
{
    return QRect(QPoint(0, 0), QtPepperMain::globalInstance()->m_screenSize);
}

QT_END_NAMESPACE
