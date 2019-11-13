#ifndef BABYLON_ANIMATIONS_EASING_POWER_EASE_H
#define BABYLON_ANIMATIONS_EASING_POWER_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class PowerEase;
using PowerEasePtr = std::shared_ptr<PowerEase>;

/**
 * @brief Easing function with a power shape (see link below).
 * @see https://easings.net/#easeInQuad
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT PowerEase : public EasingFunction {

public:
  template <typename... Ts>
  static PowerEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<PowerEase>(new PowerEase(std::forward<Ts>(args)...));
  }
  ~PowerEase(); // = default

  /**
   * @brief Hidden
   */
  float easeInCore(float gradient) const override;

protected:
  /**
   * @brief Instantiates an power base easing function.
   * @see https://easings.net/#easeInQuad
   * @param power Defines the power of the function
   */
  PowerEase(float power = 2.f);

public:
  /**
   * Defines the power of the function
   */
  float power;

}; // end of class PowerEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_POWER_EASE_H
