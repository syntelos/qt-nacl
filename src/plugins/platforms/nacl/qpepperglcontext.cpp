#include "qplatformwindowformat_qpa.h"
#include "qpepperglcontext.h"
#include "pepperinstance.h"

#include <ppapi/gles2/gl2ext_ppapi.h>
#include <ppapi/cpp/dev/context_3d_dev.h>
#include <ppapi/cpp/dev/surface_3d_dev.h>
#include <qdebug.h>

QPepperGLContext::QPepperGLContext(QPepperInstance *instance)
    :m_instance(instance)
{
    QPepperInstance *m_instance;
    m_instance->pp_instance();
    m_context = 0;
    m_surface = 0;
    m_pendingFlush = false;
}

QPepperGLContext::~QPepperGLContext()
{

}

void QPepperGLContext::makeCurrent()
{
    qDebug() << "QPepperGLContext::makeCurrent";
    return;

    if (!m_context || m_context->is_null()) {
        m_context = new pp::Context3D_Dev(*m_instance, 0, pp::Context3D_Dev(), NULL);
        if (m_context->is_null()) {
            glSetCurrentContextPPAPI(0);
            return;
        }

    m_surface = new pp::Surface3D_Dev(*m_instance, 0, NULL);
    m_context->BindSurfaces(*m_surface, *m_surface);
        m_instance->BindGraphics(*m_surface);
    }
    glSetCurrentContextPPAPI(m_surface->pp_resource());
}

void QPepperGLContext::doneCurrent()
{
    qDebug() << "QPepperGLContext::doneCurrent";
    return;
    m_instance->BindGraphics(pp::Surface3D_Dev());
    m_context->BindSurfaces(pp::Surface3D_Dev(), pp::Surface3D_Dev());
    glSetCurrentContextPPAPI(0);
}

namespace {
    void FlushCallback(void* data, int32_t result) {
        Q_UNUSED(result);
        static_cast<QPepperGLContext*>(data)->flushCallback();
    }
}

void QPepperGLContext::swapBuffers()
{
    qDebug() << "QPepperGLContext::swapBuffers";
    return;
    if (m_pendingFlush)
        return;

    m_pendingFlush = true;
   // m_surface->SwapBuffers(pp::CompletionCallback(&FlushCallback, this));
}

void QPepperGLContext::flushCallback()
{
    m_pendingFlush = false;
}

void* QPepperGLContext::getProcAddress(const QString& procName)
{
    qDebug() << "QPepperGLContext::getProcAddress" << procName;
   // const PPB_OpenGLES2_Dev* functionPointers = glGetInterfacePPAPI();

    if (procName == QLatin1String("glIsRenderbufferEXT")) {
     //   return reinterpret_cast<void *>(functionPointers->IsRenderbuffer);
    }
    // ### ... and so on for all functions.

    return 0;
}

QPlatformWindowFormat QPepperGLContext::platformWindowFormat() const
{
    qDebug() << "QPepperGLContext::platformWindowFormat";
    // ## which features?
    return QPlatformWindowFormat::DoubleBuffer | QPlatformWindowFormat::DepthBuffer;
}
