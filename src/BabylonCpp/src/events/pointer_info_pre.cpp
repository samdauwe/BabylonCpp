#include <babylon/events/pointer_info_pre.h>

namespace BABYLON {

PointerInfoPre::PointerInfoPre(PointerEventTypes _type,
                               const PointerEvent& event, float localX,
                               float localY)
    : PointerInfoBase(_type, event)
    , localPosition{Vector2(localX, localY)}
    , skipOnPointerObservable{false}
    , ray{std::nullopt}
{
}

PointerInfoPre::PointerInfoPre(PointerEventTypes _type,
                               const MouseWheelEvent& event, float localX,
                               float localY)
    : PointerInfoBase(_type, event)
    , localPosition{Vector2(localX, localY)}
    , skipOnPointerObservable{false}
    , ray{std::nullopt}
{
}

PointerInfoPre::~PointerInfoPre()
{
}

} // end of namespace BABYLON
