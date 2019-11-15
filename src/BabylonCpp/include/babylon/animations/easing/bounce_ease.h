#ifndef BABYLON_ANIMATIONS_EASING_BOUNCE_EASE_H
#define BABYLON_ANIMATIONS_EASING_BOUNCE_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BounceEase;
using BounceEasePtr = std::shared_ptr<BounceEase>;

/**
 * @brief Easing function with a bouncing shape (see link below).
 * @see https://easings.net/#easeInBounce
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT BounceEase : public EasingFunction {

public:
  template <typename... Ts>
  static BounceEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<BounceEase>(
      new BounceEase(std::forward<Ts>(args)...));
  }
  ~BounceEase(); // = default

  /**
   * @brief Hidden
   */
  float easeInCore(float gradient) const override;

protected:
  /**
   * @brief Instantiates a bounce easing.
   * @see https://easings.net/#easeInBounce
   * @param bounces Defines the number of bounces
   * @param bounciness Defines the amplitude of the bounce
   */
  BounceEase(int bounces = 3, float bounciness = 2.f);

public:
  /**
   * Defines the number of bounces
   */
  float bounces;

  /**
   * Defines the amplitude of the bounce
   */
  float bounciness;

}; // end of class BounceEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_BOUNCE_EASE_H
