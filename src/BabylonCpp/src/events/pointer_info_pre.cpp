#include <babylon/events/pointer_info_pre.h>

namespace BABYLON {

PointerInfoPre::PointerInfoPre(PointerEventTypes _type, const IPointerEvent& event, float localX,
                               float localY)
    : PointerInfoBase(_type, event)
    , ray{std::nullopt}
    , localPosition{Vector2(localX, localY)}
    , skipOnPointerObservable{false}
{
}

PointerInfoPre::PointerInfoPre(PointerEventTypes _type, const IWheelEvent& event, float localX,
                               float localY)
    : PointerInfoBase(_type, event)
    , ray{std::nullopt}
    , localPosition{Vector2(localX, localY)}
    , skipOnPointerObservable{false}
{
}

PointerInfoPre::~PointerInfoPre() = default;

} // end of namespace BABYLON
