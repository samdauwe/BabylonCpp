#include <babylon/events/pointer_info_base.h>

namespace BABYLON {

PointerInfoBase::PointerInfoBase(PointerEventTypes iType, const IPointerEvent& event)
    : type{iType}, pointerEvent{event}
{
}

PointerInfoBase::PointerInfoBase(PointerEventTypes iType, const IWheelEvent& event)
    : type{iType}, mouseWheelEvent{event}
{
}

PointerInfoBase::~PointerInfoBase() = default;

} // end of namespace BABYLON
