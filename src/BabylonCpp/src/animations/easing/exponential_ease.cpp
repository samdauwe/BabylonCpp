#include <babylon/animations/easing/exponential_ease.h>

namespace BABYLON {

ExponentialEase::ExponentialEase(int exponent)
    : EasingFunction{}, _exponent{static_cast<float>(exponent)}
{
}

ExponentialEase::~ExponentialEase()
{
}

float ExponentialEase::easeInCore(float gradient) const
{
  if (_exponent <= 0) {
    return gradient;
  }

  return ((std::exp(_exponent * gradient) - 1.f)
          / (std::exp(_exponent) - 1.f));
}

} // end of namespace BABYLON
