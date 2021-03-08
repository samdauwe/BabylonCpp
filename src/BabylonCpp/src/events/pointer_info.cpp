#include <babylon/events/pointer_info.h>

namespace BABYLON {

PointerInfo::PointerInfo(PointerEventTypes iType, const IPointerEvent& event,
                         const PickingInfo& _pickInfo)
    : PointerInfoBase(iType, event), pickInfo{_pickInfo}
{
}

PointerInfo::PointerInfo(PointerEventTypes iType, const IWheelEvent& event,
                         const PickingInfo& _pickInfo)
    : PointerInfoBase(iType, event), pickInfo{_pickInfo}
{
}

PointerInfo::~PointerInfo() = default;

} // end of namespace BABYLON
