#ifndef BABYLON_ANIMATIONS_ANIMATION_KEY_H
#define BABYLON_ANIMATIONS_ANIMATION_KEY_H

#include <babylon/babylon_global.h>

#include <babylon/animations/animation_value.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT AnimationKey {

  AnimationKey(int frame, AnimationValue value);

  int frame;
  AnimationValue value;
  Nullable<AnimationValue> inTangent;
  Nullable<AnimationValue> outTangent;
  Nullable<AnimationValue> interpolation;

}; // end of class AnimationKey

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_KEY_H
