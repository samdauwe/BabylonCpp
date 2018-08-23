#include <babylon/animations/ianimation_key.h>

namespace BABYLON {

IAnimationKey::IAnimationKey(float iFrame, AnimationValue iValue)
    : frame{iFrame}, value{iValue}
{
}

} // end of namespace BABYLON
