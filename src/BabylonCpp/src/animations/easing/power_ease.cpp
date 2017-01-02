#include <babylon/animations/easing/power_ease.h>

namespace BABYLON {

PowerEase::PowerEase(float IPower) : EasingFunction{}, power{IPower}
{
}

PowerEase::~PowerEase()
{
}

float PowerEase::easeInCore(float gradient) const
{
  return std::pow(gradient, std::max(0.f, power));
}

} // end of namespace BABYLON
