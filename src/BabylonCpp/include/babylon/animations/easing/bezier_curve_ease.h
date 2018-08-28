#ifndef BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H
#define BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BezierCurveEase : public EasingFunction {

public:
  template <typename... Ts>
  static BezierCurveEasePtr New(Ts&&... args)
  {
    return shared_ptr_t<BezierCurveEase>(
      new BezierCurveEase(::std::forward<Ts>(args)...));
  }
  ~BezierCurveEase() override;

  float easeInCore(float gradient) const override;

protected:
  BezierCurveEase(float x1 = 0.f, float y1 = 0.f, float x2 = 1.f,
                  float y2 = 1.f);

public:
  float x1;
  float y1;
  float x2;
  float y2;

}; // end of class BezierCurveEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H
