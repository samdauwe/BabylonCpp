#ifndef BABYLON_ANIMATIONS_EASING_BACK_EASE_H
#define BABYLON_ANIMATIONS_EASING_BACK_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BackEase;
using BackEasePtr = std::shared_ptr<BackEase>;

/**
 * @brief Easing function with a ease back shape (see link below).
 * @see https://easings.net/#easeInBack
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT BackEase : public EasingFunction {

public:
  template <typename... Ts>
  static BackEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<BackEase>(new BackEase(std::forward<Ts>(args)...));
  }
  ~BackEase() override; // = default

  /**
   * @brief Hidden
   */
  float easeInCore(float gradient) const override;

protected:
  /**
   * @brief Instantiates a back ease easing
   * @see https://easings.net/#easeInBack
   * @param amplitude Defines the amplitude of the function
   */
  BackEase(float amplitude = 1.f);

public:
  /**
   * Defines the amplitude of the function
   */
  float amplitude;

}; // end of class BackEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_BACK_EASE_H
