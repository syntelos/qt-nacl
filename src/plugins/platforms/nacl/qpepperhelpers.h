#ifndef QPEPPERHELPERS_H
#define QPEPPERHELPERS_H

#include <ppapi/cpp/rect.h>
#include <QRect>
#include <QAtomicInt>

pp::Rect toPPRect(const QRect rect);
QSize toQSize(pp::Size size);
pp::Size toPPSize(const QSize &size);

// QAtomic-based ref count for the callback factory.
class ThreadSafeRefCount
{
public:
    ThreadSafeRefCount();
    int32_t AddRef();
    int32_t Release();
    QAtomicInt ref;
};


#endif
