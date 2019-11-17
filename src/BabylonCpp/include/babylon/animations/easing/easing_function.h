#ifndef BABYLON_ANIMATIONS_EASING_EASING_FUNCTION_H
#define BABYLON_ANIMATIONS_EASING_EASING_FUNCTION_H

#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Base class used for every default easing function.
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT EasingFunction : public IEasingFunction {

public:
  /**
   * Interpolation follows the mathematical formula associated with the easing
   * function.
   */
  static constexpr unsigned int EASINGMODE_EASEIN = 0;

  /**
   * Interpolation follows 100% interpolation minus the output of the formula
   * associated with the easing function.
   */
  static constexpr unsigned int EASINGMODE_EASEOUT = 1;

  /**
   * Interpolation uses EaseIn for the first half of the animation and EaseOut
   * for the second half.
   */
  static constexpr unsigned int EASINGMODE_EASEINOUT = 2;

public:
  EasingFunction();
  ~EasingFunction() override; // = default

  /**
   * @brief Sets the easing mode of the current function.
   * @param easingMode Defines the willing mode (EASINGMODE_EASEIN,
   * EASINGMODE_EASEOUT or EASINGMODE_EASEINOUT)
   */
  void setEasingMode(unsigned int easingMode);

  /**
   * @brief Gets the current easing mode.
   * @returns the easing mode
   */
  unsigned int getEasingMode() const;

  /**
   * @brief Hidden
   */
  virtual float easeInCore(float gradient) const = 0;

  /**
   * @brief Given an input gradient between 0 and 1, this returns the
   * corresponding value of the easing function.
   * @param gradient Defines the value between 0 and 1 we want the easing value
   * for
   * @returns the corresponding value on the curve defined by the easing
   * function
   */
  float ease(float gradient) override;

private:
  unsigned int _easingMode;

}; // end of class EasingFunction

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_EASING_FUNCTION_H
