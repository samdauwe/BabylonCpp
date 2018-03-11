#include <babylon/animations/ianimation_key.h>

namespace BABYLON {

IAnimationKey::IAnimationKey(int iFrame, AnimationValue iValue)
    : frame{iFrame}, value{iValue}
{
}

} // end of namespace BABYLON
