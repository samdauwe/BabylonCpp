#ifndef BABYLON_ANIMATIONS_IANIMATION_KEY_H
#define BABYLON_ANIMATIONS_IANIMATION_KEY_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines an interface which represents an animation key frame.
 */
struct BABYLON_SHARED_EXPORT IAnimationKey {

  IAnimationKey();
  IAnimationKey(float frame, AnimationValue value);
  IAnimationKey(float frame, AnimationValue value,
                const std::optional<AnimationValue>& inTangent,
                const std::optional<AnimationValue>& outTangent,
                const std::optional<AnimationValue>& interpolation);
  IAnimationKey(const IAnimationKey& other);
  IAnimationKey(IAnimationKey&& other);
  IAnimationKey& operator=(const IAnimationKey& other);
  IAnimationKey& operator=(IAnimationKey&& other);
  ~IAnimationKey(); // = default

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
