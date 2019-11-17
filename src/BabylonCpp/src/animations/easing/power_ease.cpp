#include <babylon/animations/easing/power_ease.h>

#include <cmath>

#include <algorithm>

namespace BABYLON {

PowerEase::PowerEase(float IPower) : power{IPower}
{
}

PowerEase::~PowerEase() = default;

float PowerEase::easeInCore(float gradient) const
{
  return std::pow(gradient, std::max(0.f, power));
}

} // end of namespace BABYLON
