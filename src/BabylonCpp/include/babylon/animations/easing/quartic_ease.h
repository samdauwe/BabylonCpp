#ifndef BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class QuarticEase;
using QuarticEasePtr = std::shared_ptr<QuarticEase>;

/**
 * @brief Easing function with a power of 4 shape (see link below).
 * @see https://easings.net/#easeInQuart
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT QuarticEase : public EasingFunction {

public:
  template <typename... Ts>
  static QuarticEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<QuarticEase>(new QuarticEase(std::forward<Ts>(args)...));
  }
  ~QuarticEase() override; // = default

  /**
   * @brief Hidden
   */
  [[nodiscard]] float easeInCore(float gradient) const override;

protected:
  QuarticEase() = default;

}; // end of class QuarticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H
