#ifndef BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT QuarticEase : public EasingFunction {

public:
  QuarticEase();
  ~QuarticEase();

  float easeInCore(float gradient) const override;

}; // end of class QuarticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H
