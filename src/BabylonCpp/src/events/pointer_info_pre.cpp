#include <babylon/events/pointer_info_pre.h>

namespace BABYLON {

PointerInfoPre::PointerInfoPre(PointerEventTypes _type,
                               const PointerEvent& event, float localX,
                               float localY)
    : PointerInfoBase(_type, event)
    , ray{std::nullopt}
    , localPosition{Vector2(localX, localY)}
    , skipOnPointerObservable{false}
{
}

PointerInfoPre::PointerInfoPre(PointerEventTypes _type,
                               const MouseWheelEvent& event, float localX,
                               float localY)
    : PointerInfoBase(_type, event)
    , ray{std::nullopt}
    , localPosition{Vector2(localX, localY)}
    , skipOnPointerObservable{false}
{
}

PointerInfoPre::~PointerInfoPre()
{
}

} // end of namespace BABYLON
