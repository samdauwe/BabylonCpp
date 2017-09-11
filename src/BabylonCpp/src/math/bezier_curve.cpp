#include <babylon/math/bezier_curve.h>

namespace BABYLON {

float BezierCurve::Interpolate(float t, float x1, float y1, float x2, float y2)
{
  // Extract X (which is equal to time here)
  float f0 = 1.f - 3.f * x2 + 3.f * x1;
  float f1 = 3.f * x2 - 6.f * x1;
  float f2 = 3.f * x1;

  float refinedT = t;
  for (unsigned int i = 0; i < 5; ++i) {
    float refinedT2 = refinedT * refinedT;
    float refinedT3 = refinedT2 * refinedT;

    float x     = f0 * refinedT3 + f1 * refinedT2 + f2 * refinedT;
    float slope = 1.f / (3.f * f0 * refinedT2 + 2.f * f1 * refinedT + f2);
    refinedT -= (x - t) * slope;
    refinedT = ::std::min(1.f, ::std::max(0.f, refinedT));
  }

  // Resolve cubic bezier for the given x
  return 3 * ::std::pow(1 - refinedT, 2.f) * refinedT * y1
         + 3 * (1 - refinedT) * ::std::pow(refinedT, 2.f) * y2
         + ::std::pow(refinedT, 3.f);
}

} // end of namespace BABYLON
