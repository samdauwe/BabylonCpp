#ifndef BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H
#define BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class ExponentialEase;
using ExponentialEasePtr = std::shared_ptr<ExponentialEase>;

class BABYLON_SHARED_EXPORT ExponentialEase : public EasingFunction {

public:
  template <typename... Ts>
  static ExponentialEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<ExponentialEase>(
      new ExponentialEase(std::forward<Ts>(args)...));
  }
  ~ExponentialEase() override;

  float easeInCore(float gradient) const override;

protected:
  ExponentialEase(int exponent = 2);

public:
  float exponent;

}; // end of class ExponentialEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H
