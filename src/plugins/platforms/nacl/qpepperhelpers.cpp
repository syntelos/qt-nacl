#include "qpepperhelpers.h"

pp::Rect toPPRect(const QRect rect)
{
    return pp::Rect(rect.x(), rect.y(), rect.size().width(), rect.size().height());
}

QRect toQRect(pp::Rect rect)
{
    return QRect(rect.x(), rect.y(), rect.width(), rect.height());
}

QSize toQSize(pp::Size size)
{
    return QSize(size.width(), size.height());
}

pp::Size toPPSize(const QSize &size)
{
    return pp::Size(size.width(), size.height());
}

ThreadSafeRefCount::ThreadSafeRefCount()
   : ref(0) { }

int32_t ThreadSafeRefCount::AddRef()
{
    return ref.fetchAndAddOrdered(1);
}

int32_t ThreadSafeRefCount::Release()
{
    return ref.fetchAndAddOrdered(-1);
}




