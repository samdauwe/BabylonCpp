#ifndef BABYLON_ANIMATIONS_IANIMATION_STATE_H
#define BABYLON_ANIMATIONS_IANIMATION_STATE_H

#include <optional>

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _IAnimationState {
  int key;
  int repeatCount;
  std::optional<AnimationValue> workValue = std::nullopt;
  unsigned int loopMode;
  AnimationValue offsetValue;
  AnimationValue highLimitValue;
}; // end of struct _IAnimationState

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_IANIMATION_STATE_H
