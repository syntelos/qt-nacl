/*
    Licensed under the MIT license.
*/
#include "peppereventtranslator.h"
#include "3rdparty/keyboard_codes_posix.h"
#include <QWindowSystemInterface>
#include "ppapi/cpp/point.h"
#include "ppapi/cpp/var.h"

#ifndef QT_NO_PEPPER_INTEGRATION

PepperEventTranslator::PepperEventTranslator()
{
}

bool PepperEventTranslator::processEvent(const pp::InputEvent& event)
{
    switch (event.GetType()) {
        case PP_INPUTEVENT_TYPE_MOUSEDOWN:
        case PP_INPUTEVENT_TYPE_MOUSEUP:
        case PP_INPUTEVENT_TYPE_MOUSEMOVE:
            return processMouseEvent(pp::MouseInputEvent(event), event.GetType());
        break;

        //case PP_INPUTEVENT_TYPE_MOUSEENTER:
        //case PP_INPUTEVENT_TYPE_MOUSELEAVE:

        case PP_INPUTEVENT_TYPE_WHEEL:
            return processWheelEvent(pp::WheelInputEvent(event));
        break;

        //case PP_INPUTEVENT_TYPE_RAWKEYDOWN:

        case PP_INPUTEVENT_TYPE_KEYDOWN:
        case PP_INPUTEVENT_TYPE_KEYUP:
            return processKeyEvent(pp::KeyboardInputEvent(event), event.GetType());
        break;

        case PP_INPUTEVENT_TYPE_CHAR:
            return processCharacterEvent(pp::KeyboardInputEvent(event));
        break;

        default:
        break;
    }

    return false;
}

bool PepperEventTranslator::processMouseEvent(const pp::MouseInputEvent &event, PP_InputEvent_Type eventType)
{
    //qDebug() << "processMouseEvent" << event.GetPosition().x() << event.GetPosition().y();
    QPoint point(event.GetPosition().x(), event.GetPosition().y());
    //Qt::MouseButton button = translatePepperMouseButton(event.button);
    Qt::MouseButtons modifiers = translatePepperMouseModifiers(event.GetModifiers());

    // Qt mouse button state is state *after* the mouse event, send NoButton
    // on mouse up.
    // ### strictly not correct, only the state for the released button should
    // be cleared.
    if (eventType == PP_INPUTEVENT_TYPE_MOUSEUP) {
        QWindowSystemInterface::handleMouseEvent(widget, point, point, Qt::MouseButtons(Qt::NoButton));
    } else {
        QWindowSystemInterface::handleMouseEvent(widget, point, point, modifiers);
    }

    return true;
}

bool PepperEventTranslator::processWheelEvent(const pp::WheelInputEvent &event)
{
    if (event.GetTicks().x() != 0) {
      //  QWindowSystemInterface::handleWheelEvent(0, QPoint(), QPoint(), event.wheel_ticks_x, Qt::Horizontal);
    }
    if (event.GetTicks().y()!= 0) {
     //   QWindowSystemInterface::handleWheelEvent(0, QPoint(), QPoint(), event.wheel_ticks_y, Qt::Vertical);
    }
    return true;
}

/*
    Key translation: Pepper sends three types of events. Pressing and
    holding a key gives the following sequence:
    NPEventType_KeyDown
    NPEventType_Char
    NPEventType_KeyDown
    NPEventType_Char
    ...
    NPEventType_KeyUp

    Translate this to Qt KeyPress/KeyRelease events by sending
    the Press on NPEventType_Char.
*/
bool PepperEventTranslator::processKeyEvent(const pp::KeyboardInputEvent &event, PP_InputEvent_Type eventType)
{
    Qt::KeyboardModifiers modifiers = translatePepperKeyModifiers(event.GetModifiers());
    bool alphanumretic;
    Qt::Key key = translatePepperKey(event.GetKeyCode(), &alphanumretic);

    if (eventType == PP_INPUTEVENT_TYPE_KEYDOWN) {
        currentPepperKey = event.GetKeyCode();
        if (!alphanumretic) {
            QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyPress, key, modifiers);
           // qDebug() << "send Key Down" << event.GetKeyCode() << hex << modifiers;
        }
    }

    if (eventType == PP_INPUTEVENT_TYPE_KEYUP) {
       // qDebug() << "send Key Up" << event.key_code << hex << modifiers;
        QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyRelease, key,  modifiers);
    }
    return true;
}

bool PepperEventTranslator::processCharacterEvent(const pp::KeyboardInputEvent &event)
{
    QString text = QString::fromUtf8(event.GetCharacterText().AsString().c_str()); // ### wide characters?
    Qt::KeyboardModifiers modifiers = translatePepperKeyModifiers(event.GetModifiers());
    Qt::Key key = translatePepperKey(currentPepperKey, 0);
    QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyPress, key, modifiers, text);

    return true;
}

Qt::MouseButton PepperEventTranslator::translatePepperMouseButton(PP_InputEvent_MouseButton pepperButton)
{
    Qt::MouseButton button;
    if (pepperButton == PP_INPUTEVENT_MOUSEBUTTON_LEFT)
        button = Qt::LeftButton;
    if (pepperButton == PP_INPUTEVENT_MOUSEBUTTON_MIDDLE)
        button = Qt::MidButton;
    if (pepperButton == PP_INPUTEVENT_MOUSEBUTTON_RIGHT)
        button = Qt::RightButton;
    return button;
}

Qt::MouseButtons PepperEventTranslator::translatePepperMouseModifiers(uint32_t modifier)
{
    Qt::MouseButtons buttons;
    if (modifier & PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN)
        buttons |= Qt::LeftButton;
    if (modifier & PP_INPUTEVENT_MODIFIER_RIGHTBUTTONDOWN)
        buttons |= Qt::RightButton;
    if (modifier & PP_INPUTEVENT_MODIFIER_MIDDLEBUTTONDOWN)
        buttons |= Qt::MidButton;

    return buttons;
}

/*
    Translate key codes. Some of the codes match (0..1, A..Z),
    so copy the value over by default and then handle the rest
    as special cases.

    This implementation is incomplete - most used keys are
    added first.
*/

Qt::Key PepperEventTranslator::translatePepperKey(uint32_t pepperKey, bool *outAlphanumretic)
{
    Qt::Key qtKey;
    if (outAlphanumretic)
        *outAlphanumretic = false;

    using namespace base; // keybaord codes from keyboard_codes_posix.h

    switch (pepperKey) {
        // (in VKEY order)
        case VKEY_BACK: qtKey = Qt::Key_Backspace; break;
        case VKEY_TAB: qtKey = Qt::Key_Tab; break;
        case VKEY_CLEAR : qtKey = Qt::Key_Clear; break;
        case VKEY_RETURN: qtKey = Qt::Key_Return; break;
        case VKEY_SHIFT: qtKey = Qt::Key_Shift; break;
        case VKEY_CONTROL: qtKey = Qt::Key_Control; break;
        case VKEY_MENU: qtKey = Qt::Key_Menu; break;
        case VKEY_PAUSE: qtKey = Qt::Key_Pause; break;
        case VKEY_SPACE: qtKey = Qt::Key_Space; break;
        case VKEY_END: qtKey = Qt::Key_End; break;
        case VKEY_HOME: qtKey = Qt::Key_Home; break;
        case VKEY_LEFT: qtKey = Qt::Key_Left; break;
        case VKEY_UP: qtKey = Qt::Key_Up; break;
        case VKEY_RIGHT: qtKey = Qt::Key_Right; break;
        case VKEY_DOWN: qtKey = Qt::Key_Down; break;
        case VKEY_SELECT: qtKey = Qt::Key_Select; break;
        case VKEY_PRINT: qtKey = Qt::Key_Print; break;
        case VKEY_EXECUTE: qtKey = Qt::Key_Execute; break;
        //case VKEY_SNAPSHOT: qtKey = Qt::Key_; break;
        case VKEY_INSERT: qtKey = Qt::Key_Insert; break;
        case VKEY_DELETE: qtKey = Qt::Key_Delete; break;
        case VKEY_HELP: qtKey = Qt::Key_Help; break;
        case VKEY_NUMPAD0: qtKey = Qt::Key_0; break;
        case VKEY_NUMPAD1: qtKey = Qt::Key_1; break;
        case VKEY_NUMPAD2: qtKey = Qt::Key_2; break;
        case VKEY_NUMPAD3: qtKey = Qt::Key_3; break;
        case VKEY_NUMPAD4: qtKey = Qt::Key_4; break;
        case VKEY_NUMPAD5: qtKey = Qt::Key_5; break;
        case VKEY_NUMPAD6: qtKey = Qt::Key_6; break;
        case VKEY_NUMPAD7: qtKey = Qt::Key_7; break;
        case VKEY_NUMPAD8: qtKey = Qt::Key_8; break;
        case VKEY_NUMPAD9: qtKey = Qt::Key_9; break;
        case VKEY_F1: qtKey = Qt::Key_F1; break;
        case VKEY_F2: qtKey = Qt::Key_F2; break;
        case VKEY_F3: qtKey = Qt::Key_F3; break;
        case VKEY_F4: qtKey = Qt::Key_F4; break;
        case VKEY_F5: qtKey = Qt::Key_F5; break;
        case VKEY_F6: qtKey = Qt::Key_F6; break;
        case VKEY_F7: qtKey = Qt::Key_F7; break;
        case VKEY_F8: qtKey = Qt::Key_F8; break;
        case VKEY_F9: qtKey = Qt::Key_F9; break;
        case VKEY_F10: qtKey = Qt::Key_F10; break;
        case VKEY_F11: qtKey = Qt::Key_F11; break;
        case VKEY_F12: qtKey = Qt::Key_F12; break;
        case VKEY_F13: qtKey = Qt::Key_F13; break;
        case VKEY_F14: qtKey = Qt::Key_F14; break;
        case VKEY_F15: qtKey = Qt::Key_F15; break;
        case VKEY_F16: qtKey = Qt::Key_F16; break;
        case VKEY_F17: qtKey = Qt::Key_F17; break;
        case VKEY_F18: qtKey = Qt::Key_F18; break;
        case VKEY_F19: qtKey = Qt::Key_F19; break;
        case VKEY_F20: qtKey = Qt::Key_F20; break;
        case VKEY_F21: qtKey = Qt::Key_F21; break;
        case VKEY_F22: qtKey = Qt::Key_F22; break;
        case VKEY_F23: qtKey = Qt::Key_F23; break;
        case VKEY_F24: qtKey = Qt::Key_F24; break;

      default:
            if (outAlphanumretic)
                *outAlphanumretic = true;
            qtKey = static_cast<Qt::Key>(pepperKey);
        break;
    }

    return qtKey;
}

/*
    Translate supported modifiers (first five pepper modifers).
    pepper starts at 0x1, Qt starts at 0x02000000. (same order)
*/
Qt::KeyboardModifiers PepperEventTranslator::translatePepperKeyModifiers(uint32_t modifier)
{
    return Qt::KeyboardModifiers((modifier & 0x1F)<< 25);
}

#endif
