#ifndef BABYLON_ANIMATIONS_TARGETED_ANIMATION_H
#define BABYLON_ANIMATIONS_TARGETED_ANIMATION_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Animation;
class IAnimatable;
using AnimationPtr   = std::shared_ptr<Animation>;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

/**
 * @brief This class defines the direct association between an animation and a
 * target.
 */
struct BABYLON_SHARED_EXPORT TargetedAnimation {
  AnimationPtr animation = nullptr;
  IAnimatablePtr target  = nullptr;
}; // end of class TargetedAnimation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_TARGETED_ANIMATION_H
