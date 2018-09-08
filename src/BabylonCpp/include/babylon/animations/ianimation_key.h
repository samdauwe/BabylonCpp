#ifndef BABYLON_ANIMATIONS_IANIMATION_KEY_H
#define BABYLON_ANIMATIONS_IANIMATION_KEY_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines an interface which represents an animation key frame.
 */
struct BABYLON_SHARED_EXPORT IAnimationKey {

  IAnimationKey(float frame, AnimationValue value);

  /**
   * Frame of the key frame
   */
  float frame;

  /**
   * Value at the specifies key frame
   */
  AnimationValue value;

  /**
   * The input tangent for the cubic hermite spline
   */
  std::optional<AnimationValue> inTangent;

  /**
   * The output tangent for the cubic hermite spline
   */
  std::optional<AnimationValue> outTangent;

  /**
   * The animation interpolation type
   */
  std::optional<AnimationValue> interpolation;

}; // end of struct AnimationKey

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_IANIMATION_KEY_H
