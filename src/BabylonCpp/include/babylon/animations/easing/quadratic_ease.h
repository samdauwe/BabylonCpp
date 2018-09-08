#ifndef BABYLON_ANIMATIONS_EASING_QUADRATIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_QUADRATIC_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class QuadraticEase;
using QuadraticEasePtr = std::shared_ptr<QuadraticEase>;

class BABYLON_SHARED_EXPORT QuadraticEase : public EasingFunction {

public:
  template <typename... Ts>
  static QuadraticEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<QuadraticEase>(
      new QuadraticEase(std::forward<Ts>(args)...));
  }
  ~QuadraticEase() override;

  float easeInCore(float gradient) const override;

protected:
  QuadraticEase();

}; // end of class QuadraticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_QUADRATIC_EASE_H
