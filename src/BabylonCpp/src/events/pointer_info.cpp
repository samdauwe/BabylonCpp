#include <babylon/events/pointer_info.h>

namespace BABYLON {

PointerInfo::PointerInfo(PointerEventTypes iType, const PointerEvent& event,
                         const PickingInfo& _pickInfo)
    : PointerInfoBase(iType, event), pickInfo{_pickInfo}
{
}

PointerInfo::PointerInfo(PointerEventTypes iType, const MouseWheelEvent& event,
                         const PickingInfo& _pickInfo)
    : PointerInfoBase(iType, event), pickInfo{_pickInfo}
{
}

PointerInfo::~PointerInfo()
{
}

} // end of namespace BABYLON
