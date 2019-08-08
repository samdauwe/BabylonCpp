#ifndef BABYLON_ANIMATIONS_EASING_IEASING_FUNCTION_H
#define BABYLON_ANIMATIONS_EASING_IEASING_FUNCTION_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This represents the main contract an easing function should follow.
 * Easing functions are used throughout the animation system.
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
struct BABYLON_SHARED_EXPORT IEasingFunction {
  virtual ~IEasingFunction() = default;
  /**
   * @brief Given an input gradient between 0 and 1, this returns the
   * corrseponding value of the easing function. The link below provides some of
   * the most common examples of easing functions.
   * @see https://easings.net/
   * @param gradient Defines the value between 0 and 1 we want the easing value
   * for
   * @returns the corresponding value on the curve defined by the easing
   * function
   */
  virtual float ease(float gradient) = 0;

}; // end of struct IEasingFunction

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_IEASING_FUNCTION_H
