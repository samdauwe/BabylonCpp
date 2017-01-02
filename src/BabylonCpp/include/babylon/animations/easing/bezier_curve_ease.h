#ifndef BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H
#define BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BezierCurveEase : public EasingFunction {

public:
  BezierCurveEase(float x1 = 0.f, float y1 = 0.f, float x2 = 1.f,
                  float y2 = 1.f);
  ~BezierCurveEase();

  float easeInCore(float gradient) const override;

public:
  float x1;
  float y1;
  float x2;
  float y2;

}; // end of class BezierCurveEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H
