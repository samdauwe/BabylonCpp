#ifndef BABYLON_ANIMATIONS_EASING_POWER_EASE_H
#define BABYLON_ANIMATIONS_EASING_POWER_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class PowerEase;
using PowerEasePtr = std::shared_ptr<PowerEase>;

class BABYLON_SHARED_EXPORT PowerEase : public EasingFunction {

public:
  template <typename... Ts>
  static PowerEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<PowerEase>(new PowerEase(std::forward<Ts>(args)...));
  }
  ~PowerEase() override;

  float easeInCore(float gradient) const override;

protected:
  PowerEase(float power = 2.f);

public:
  float power;

}; // end of class PowerEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_POWER_EASE_H
