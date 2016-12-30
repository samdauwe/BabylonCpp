#ifndef BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H
#define BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CircleEase : public EasingFunction {

public:
  CircleEase();
  ~CircleEase();

  float easeInCore(float gradient) const override;

}; // end of class CircleEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H
