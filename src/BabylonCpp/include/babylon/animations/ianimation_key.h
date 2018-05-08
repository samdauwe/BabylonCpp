#ifndef BABYLON_ANIMATIONS_IANIMATION_KEY_H
#define BABYLON_ANIMATIONS_IANIMATION_KEY_H

#include <babylon/babylon_global.h>

#include <babylon/animations/animation_value.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

/**
 * @brief Defines an interface which represents an animation key frame.
 */
struct BABYLON_SHARED_EXPORT IAnimationKey {

  IAnimationKey(int frame, AnimationValue value);

  /**
   * Frame of the key frame
   */
  int frame;

  /**
   * Value at the specifies key frame
   */
  AnimationValue value;

  /**
   * The input tangent for the cubic hermite spline
   */
  Nullable<AnimationValue> inTangent;

  /**
   * The output tangent for the cubic hermite spline
   */
  Nullable<AnimationValue> outTangent;

  /**
   * The animation interpolation type
   */
  Nullable<AnimationValue> interpolation;

}; // end of struct AnimationKey

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_IANIMATION_KEY_H
