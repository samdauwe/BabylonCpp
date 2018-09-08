#include <babylon/animations/easing/exponential_ease.h>

#include <cmath>

namespace BABYLON {

ExponentialEase::ExponentialEase(int iExponent)
    : EasingFunction{}, exponent{static_cast<float>(iExponent)}
{
}

ExponentialEase::~ExponentialEase()
{
}

float ExponentialEase::easeInCore(float gradient) const
{
  if (exponent <= 0) {
    return gradient;
  }

  return ((std::exp(exponent * gradient) - 1.f) / (std::exp(exponent) - 1.f));
}

} // end of namespace BABYLON
