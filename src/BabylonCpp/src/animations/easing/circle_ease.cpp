#include <babylon/animations/easing/circle_ease.h>

#include <cmath>

#include <algorithm>

namespace BABYLON {

CircleEase::CircleEase() : EasingFunction{}
{
}

CircleEase::~CircleEase()
{
}

float CircleEase::easeInCore(float gradient) const
{
  float _gradient = std::max(0.f, std::min(1.f, gradient));
  return (1.f - std::sqrt(1.f - (_gradient * _gradient)));
}

} // end of namespace BABYLON
