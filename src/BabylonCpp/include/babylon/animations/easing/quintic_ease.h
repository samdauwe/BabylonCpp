#ifndef BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT QuinticEase : public EasingFunction {

public:
  QuinticEase();
  ~QuinticEase();

  float easeInCore(float gradient) const override;

}; // end of class QuinticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H
