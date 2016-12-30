#include <babylon/animations/easing/bounce_ease.h>

namespace BABYLON {

BounceEase::BounceEase(int bounces, float bounciness)
    : EasingFunction{},
      _bounces{static_cast<float>(bounces)},
      _bounciness{bounciness}
{
}

BounceEase::~BounceEase()
{
}

float BounceEase::easeInCore(float gradient) const
{
  float y          = std::fmax(0.f, _bounces);
  float bounciness = _bounciness;
  if (bounciness <= 1.f) {
    bounciness = 1.001f;
  }
  float num9  = std::pow(bounciness, y);
  float num5  = 1.f - bounciness;
  float num4  = ((1.f - num9) / num5) + (num9 * 0.5f);
  float num15 = gradient * num4;
  float num65
    = std::log((-num15 * (1.f - bounciness)) + 1.f) / std::log(bounciness);
  float num3  = std::floor(num65);
  float num13 = num3 + 1.f;
  float num8  = (1.f - std::pow(bounciness, num3)) / (num5 * num4);
  float num12 = (1.f - std::pow(bounciness, num13)) / (num5 * num4);
  float num7  = (num8 + num12) * 0.5f;
  float num6  = gradient - num7;
  float num2 = num7 - num8;
  return (
    ((-std::pow(1.f / bounciness, y - num3) / (num2 * num2)) * (num6 - num2))
    * (num6 + num2));
}

} // end of namespace BABYLON
