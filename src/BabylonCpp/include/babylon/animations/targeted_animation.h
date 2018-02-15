#ifndef BABYLON_ANIMATIONS_TARGETED_ANIMATION_H
#define BABYLON_ANIMATIONS_TARGETED_ANIMATION_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief This class defines the direct association between an animation and a
 * target.
 */
struct BABYLON_SHARED_EXPORT TargetedAnimation {
  Animation* animation = nullptr;
  IAnimatable* target  = nullptr;
}; // end of class TargetedAnimation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_TARGETED_ANIMATION_H
