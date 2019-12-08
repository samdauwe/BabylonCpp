#ifndef BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class QuinticEase;
using QuinticEasePtr = std::shared_ptr<QuinticEase>;

/**
 * @brief Easing function with a power of 5 shape (see link below).
 * @see https://easings.net/#easeInQuint
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT QuinticEase : public EasingFunction {

public:
  template <typename... Ts>
  static QuinticEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<QuinticEase>(new QuinticEase(std::forward<Ts>(args)...));
  }
  ~QuinticEase() override; // = default

  /**
   * @brief Hidden
   */
  [[nodiscard]] float easeInCore(float gradient) const override;

protected:
  QuinticEase() = default;

}; // end of class QuinticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H
