#include <babylon/engine/pointer_info_pre.h>

namespace BABYLON {

PointerInfoPre::PointerInfoPre(unsigned int _type, const PointerEvent& event,
                               float localX, float localY)
    : PointerInfoBase(_type, event)
    , localPosition{Vector2(localX, localY)}
    , skipOnPointerObservable{false}
{
}

PointerInfoPre::PointerInfoPre(unsigned int _type, const MouseWheelEvent& event,
                               float localX, float localY)
    : PointerInfoBase(_type, event)
    , localPosition{Vector2(localX, localY)}
    , skipOnPointerObservable{false}
{
}

PointerInfoPre::~PointerInfoPre()
{
}

} // end of namespace BABYLON
