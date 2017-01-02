#ifndef BABYLON_ANIMATIONS_EASING_BACK_EASE_H
#define BABYLON_ANIMATIONS_EASING_BACK_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BackEase : public EasingFunction {

public:
  BackEase(float amplitude = 1.f);
  ~BackEase();

  float easeInCore(float gradient) const override;

public:
  float amplitude;

}; // end of class BackEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_BACK_EASE_H
