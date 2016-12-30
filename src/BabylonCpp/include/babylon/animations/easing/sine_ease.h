#ifndef BABYLON_ANIMATIONS_EASING_SINE_EASE_H
#define BABYLON_ANIMATIONS_EASING_SINE_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SineEase : public EasingFunction {

public:
  SineEase();
  ~SineEase();

  float easeInCore(float gradient) const override;

}; // end of class SineEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_SINE_EASE_H
