#ifndef BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H
#define BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class ExponentialEase;
using ExponentialEasePtr = std::shared_ptr<ExponentialEase>;

/**
 * @brief Easing function with an exponential shape (see link below).
 * @see https://easings.net/#easeInExpo
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT ExponentialEase : public EasingFunction {

public:
  template <typename... Ts>
  static ExponentialEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<ExponentialEase>(
      new ExponentialEase(std::forward<Ts>(args)...));
  }
  ~ExponentialEase(); // = default

  /**
   * @brief Hidden
   */
  float easeInCore(float gradient) const override;

protected:
  /**
   * @brief Instantiates an exponential easing function.
   * @see https://easings.net/#easeInExpo
   * @param exponent Defines the exponent of the function
   */
  ExponentialEase(int exponent = 2);

public:
  /**
   * Defines the exponent of the function
   */
  float exponent;

}; // end of class ExponentialEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H
