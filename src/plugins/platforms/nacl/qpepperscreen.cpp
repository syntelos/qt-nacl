/*
    Licensed under the MIT license.
*/
#include "qpepperscreen.h"
#include "qpeppermain.h"
#include "qpepperhelpers.h"

QT_BEGIN_NAMESPACE

/*
    A pepper screen corresponds to the qt div tag
*/

QPepperScreen::QPepperScreen()
: m_depth(32), m_format(QImage::Format_ARGB32_Premultiplied)
{
}

QRect QPepperScreen::geometry() const
{
    QPepperInstance *instance = QtPepperMain::get()->m_mainInstance;
    if (!instance)
        return QRect();
    return toQRect(QtPepperMain::get()->m_mainInstance->m_currentGeometry);
}

QT_END_NAMESPACE
