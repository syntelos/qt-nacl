/*
    Licensed under the MIT license.
*/
#ifndef QT_PEPPER_INSTANCE_H
#define QT_PEPPER_INSTANCE_H

#include "peppereventtranslator.h"
#include "qpepperhelpers.h"

#include <ppapi/cpp/instance.h>
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include <ppapi/cpp/completion_callback.h>

class QPepperIntegration;
class QPepperInstance : public pp::Instance
{
public:
    QPepperInstance(PP_Instance instance);
    virtual ~QPepperInstance();

    // pp::Instance virtuals. Called from the pepper thread.
    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
    virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip);
    virtual void DidChangeFocus(bool has_focus);
    virtual bool HandleInputEvent(const pp::InputEvent& event);
    virtual bool HandleDocumentLoad(const pp::URLLoader& url_loader);
    virtual void HandleMessage(const pp::Var& message);

    // Interface for QPepperCompositor. Called from the Qt thread.
    void flush();
    void waitForFlushed();

    // privates:
    void flushFunction(int32_t);
    void flushCompletedCallback(int32_t);
    void setupTestGraphics(pp::Size newSize);
    PepperEventTranslator m_eventTranslator;

    pp::Var m_console;
    pp::Rect m_currentGeometry;

    pp::Graphics2D *m_context2D;
    pp::ImageData *m_imageData2D;
    QImage *m_frameBuffer;
    bool m_inFlush;
    qint32 m_windowId;
    pp::CompletionCallbackFactory<QPepperInstance, ThreadSafeRefCount> m_callbackFactory;
};

#endif
