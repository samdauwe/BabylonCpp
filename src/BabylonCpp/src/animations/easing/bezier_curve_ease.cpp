#include <babylon/animations/easing/bezier_curve_ease.h>

#include <babylon/math/bezier_curve.h>

namespace BABYLON {

BezierCurveEase::BezierCurveEase(float x1, float y1, float x2, float y2)
    : EasingFunction{}, _x1{x1}, _y1{y1}, _x2{x2}, _y2{y2}
{
}

BezierCurveEase::~BezierCurveEase()
{
}

float BezierCurveEase::easeInCore(float gradient) const
{
  return BezierCurve::Interpolate(gradient, _x1, _y1, _x2, _y2);
}

} // end of namespace BABYLON
