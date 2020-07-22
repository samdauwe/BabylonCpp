#ifndef BABYLON_ANIMATIONS_EASING_SINE_EASE_H
#define BABYLON_ANIMATIONS_EASING_SINE_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class SineEase;
using SineEasePtr = std::shared_ptr<SineEase>;

/**
 * @brief Easing function with a sin shape (see link below).
 * @see https://easings.net/#easeInSine
 * @see https://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT SineEase : public EasingFunction {

public:
  template <typename... Ts>
  static SineEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<SineEase>(new SineEase(std::forward<Ts>(args)...));
  }
  ~SineEase() override; // = default

  /**
   * @brief Hidden
   */
  [[nodiscard]] float easeInCore(float gradient) const override;

protected:
  SineEase() = default;

}; // end of class SineEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_SINE_EASE_H
