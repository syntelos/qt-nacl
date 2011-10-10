/*
    Licensed under the MIT license.
*/
#ifndef QT_PEPPER_INSTANCE_H
#define QT_PEPPER_INSTANCE_H

#include "peppereventtranslator.h"

#include <ppapi/cpp/instance.h>
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include <ppapi/cpp/dev/scriptable_object_deprecated.h>

class QPepperIntegration;
class QtInstance : public pp::Instance
{
public:
    QtInstance(PP_Instance instance);
    virtual ~QtInstance();

    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);

    virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip);
    virtual void DidChangeFocus(bool has_focus);
    virtual bool HandleInputEvent(const pp::InputEvent& event);
    virtual bool HandleDocumentLoad(const pp::URLLoader& url_loader);
    virtual void HandleMessage(const pp::Var& message);

    void setupGraphics(pp::Size newSize);
#ifndef QT_PEPPER_STANDALONE_MODE
    PepperEventTranslator eventTranslator;
#endif
    pp::Var m_console;
    pp::Rect currentGeometry;
};

#endif
