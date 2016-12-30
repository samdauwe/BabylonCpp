#include <babylon/engine/pointer_info_base.h>

namespace BABYLON {

PointerInfoBase::PointerInfoBase(unsigned int _type, const PointerEvent& event)
    : type{_type}, pointerEvent{event}
{
}

PointerInfoBase::PointerInfoBase(unsigned int _type,
                                 const MouseWheelEvent& event)
    : type{_type}, mouseWheelEvent{event}
{
}

PointerInfoBase::~PointerInfoBase()
{
}

} // end of namespace BABYLON
