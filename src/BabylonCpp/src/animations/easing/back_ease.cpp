#include <babylon/animations/easing/back_ease.h>

#include <cmath>

#include <babylon/babylon_constants.h>

#include <algorithm>

namespace BABYLON {

BackEase::BackEase(float iAmplitude) : EasingFunction{}, amplitude{iAmplitude}
{
}

BackEase::~BackEase() = default;

float BackEase::easeInCore(float gradient) const
{
  float num = std::max(0.f, amplitude);
  return (std::pow(gradient, 3.f)
          - ((gradient * num) * std::sin(Math::PI * gradient)));
}

} // end of namespace BABYLON
