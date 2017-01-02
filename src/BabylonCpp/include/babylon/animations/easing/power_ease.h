#ifndef BABYLON_ANIMATIONS_EASING_POWER_EASE_H
#define BABYLON_ANIMATIONS_EASING_POWER_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PowerEase : public EasingFunction {

public:
  PowerEase(float power = 2.f);
  ~PowerEase();

  float easeInCore(float gradient) const override;

public:
  float power;

}; // end of class PowerEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_POWER_EASE_H
