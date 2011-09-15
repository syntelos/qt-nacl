#ifndef QPLATFORMGLCONTEXT_PEPPER_H
#define QPLATFORMGLCONTEXT_PEPPER_H

#include <QPlatformGLContext>
class QtInstance;

namespace pp {
    class Context3D_Dev;
    class Surface3D_Dev;
}

class QPepperGLContext : public QPlatformGLContext
{
public:
    explicit QPepperGLContext(QtInstance *instance);
    virtual ~QPepperGLContext();

    virtual void makeCurrent();
    virtual void doneCurrent();
    virtual void swapBuffers();
    void flushCallback();
    virtual void* getProcAddress(const QString& procName);

    virtual QPlatformWindowFormat platformWindowFormat() const;
private:
    QtInstance *m_instance;
    pp::Context3D_Dev *m_context;
    pp::Surface3D_Dev *m_surface;
    bool m_pendingFlush;
};

#endif
