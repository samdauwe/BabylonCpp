#include <babylon/events/pointer_info_base.h>

namespace BABYLON {

PointerInfoBase::PointerInfoBase(unsigned int iType, const PointerEvent& event)
    : type{iType}, pointerEvent{event}
{
}

PointerInfoBase::PointerInfoBase(unsigned int iType,
                                 const MouseWheelEvent& event)
    : type{iType}, mouseWheelEvent{event}
{
}

PointerInfoBase::~PointerInfoBase()
{
}

} // end of namespace BABYLON
