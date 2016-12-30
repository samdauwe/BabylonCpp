#include <babylon/animations/easing/power_ease.h>

namespace BABYLON {

PowerEase::PowerEase(float power) : EasingFunction{}, _power{power}
{
}

PowerEase::~PowerEase()
{
}

float PowerEase::easeInCore(float gradient) const
{
  return std::pow(gradient, std::fmax(0.f, _power));
}

} // end of namespace BABYLON
