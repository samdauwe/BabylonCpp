#include <babylon/animations/easing/bezier_curve_ease.h>

#include <babylon/math/bezier_curve.h>

namespace BABYLON {

BezierCurveEase::BezierCurveEase(float iX1, float iY1, float iX2, float iY2)
    : EasingFunction{}, x1{iX1}, y1{iY1}, x2{iX2}, y2{iY2}
{
}

BezierCurveEase::~BezierCurveEase()
{
}

float BezierCurveEase::easeInCore(float gradient) const
{
  return BezierCurve::Interpolate(gradient, x1, y1, x2, y2);
}

} // end of namespace BABYLON
