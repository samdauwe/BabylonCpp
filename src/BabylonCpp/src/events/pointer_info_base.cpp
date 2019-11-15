#include <babylon/events/pointer_info_base.h>

namespace BABYLON {

PointerInfoBase::PointerInfoBase(PointerEventTypes iType,
                                 const PointerEvent& event)
    : type{iType}, pointerEvent{event}
{
}

PointerInfoBase::PointerInfoBase(PointerEventTypes iType,
                                 const MouseWheelEvent& event)
    : type{iType}, mouseWheelEvent{event}
{
}

PointerInfoBase::~PointerInfoBase() = default;

} // end of namespace BABYLON
