#ifndef BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H
#define BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ExponentialEase : public EasingFunction {

public:
  ExponentialEase(int exponent = 2);
  ~ExponentialEase();

  float easeInCore(float gradient) const override;

private:
  float _exponent;

}; // end of class ExponentialEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_EXPONENTIAL_EASE_H
