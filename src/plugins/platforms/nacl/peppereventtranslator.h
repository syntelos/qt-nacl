/*
    Licensed under the MIT license.
*/
#ifndef QT_PEPPEREVENT_TRANSLATOR_H
#define QT_PEPPEREVENT_TRANSLATOR_H

#ifndef QT_NO_PEPPER_INTEGRATION
#include "ppapi/c/pp_input_event.h"
#include "ppapi/cpp/input_event.h"

#include <QtGui>

class PepperEventTranslator
{
public:
    PepperEventTranslator();
    bool processEvent(const pp::InputEvent& event);

    bool processMouseEvent(const pp::MouseInputEvent &event, PP_InputEvent_Type eventType);
    bool processWheelEvent(const pp::WheelInputEvent &event);
    bool processKeyEvent(const pp::KeyboardInputEvent &event, PP_InputEvent_Type eventType);
    bool processCharacterEvent(const pp::KeyboardInputEvent &event);

    Qt::MouseButton translatePepperMouseButton(PP_InputEvent_MouseButton pepperButton);
    Qt::MouseButtons translatePepperMouseModifiers(uint32_t modifier);
    Qt::Key translatePepperKey(uint32_t pepperKey, bool *outAlphanumretic);
    Qt::KeyboardModifiers translatePepperKeyModifiers(uint32_t modifier);
private:
    uint32_t currentPepperKey;
};

#endif

#endif // PEPPEREVENTTRANSLATOR_H
