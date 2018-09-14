#include <babylon/animations/easing/bounce_ease.h>

#include <cmath>

#include <algorithm>

namespace BABYLON {

BounceEase::BounceEase(int iBounces, float iBounciness)
    : EasingFunction{}
    , bounces{static_cast<float>(iBounces)}
    , bounciness{iBounciness}
{
}

BounceEase::~BounceEase()
{
}

float BounceEase::easeInCore(float gradient) const
{
  float y           = std::max(0.f, bounces);
  float _bounciness = bounciness;
  if (_bounciness <= 1.f) {
    _bounciness = 1.001f;
  }
  float num9  = std::pow(_bounciness, y);
  float num5  = 1.f - _bounciness;
  float num4  = ((1.f - num9) / num5) + (num9 * 0.5f);
  float num15 = gradient * num4;
  float num65
    = std::log((-num15 * (1.f - _bounciness)) + 1.f) / std::log(_bounciness);
  float num3  = std::floor(num65);
  float num13 = num3 + 1.f;
  float num8  = (1.f - std::pow(_bounciness, num3)) / (num5 * num4);
  float num12 = (1.f - std::pow(_bounciness, num13)) / (num5 * num4);
  float num7  = (num8 + num12) * 0.5f;
  float num6  = gradient - num7;
  float num2  = num7 - num8;
  return (
    ((-std::pow(1.f / _bounciness, y - num3) / (num2 * num2)) * (num6 - num2))
    * (num6 + num2));
}

} // end of namespace BABYLON
