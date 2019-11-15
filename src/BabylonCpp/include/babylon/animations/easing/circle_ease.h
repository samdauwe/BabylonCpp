#ifndef BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H
#define BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class CircleEase;
using CircleEasePtr = std::shared_ptr<CircleEase>;

/**
 * @brief Easing function with a circle shape (see link below).
 * @see https://easings.net/#easeInCirc
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT CircleEase : public EasingFunction {

public:
  template <typename... Ts>
  static CircleEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<CircleEase>(
      new CircleEase(std::forward<Ts>(args)...));
  }
  ~CircleEase(); // = default

  /**
   * @brief Hidden
   */
  float easeInCore(float gradient) const override;

protected:
  CircleEase();

}; // end of class CircleEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H
