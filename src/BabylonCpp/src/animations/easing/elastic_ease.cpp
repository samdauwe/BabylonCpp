#include <babylon/animations/easing/elastic_ease.h>

namespace BABYLON {

ElasticEase::ElasticEase(int iOscillations, int iSpringiness)
    : EasingFunction{}
    , oscillations{static_cast<float>(iOscillations)}
    , springiness{static_cast<float>(iSpringiness)}
{
}

ElasticEase::~ElasticEase()
{
}

float ElasticEase::easeInCore(float gradient) const
{
  float num2 = 0.f;
  float num3 = std::max(0.f, oscillations);
  float num  = std::max(0.f, springiness);

  if (std_util::almost_equal(num, 0.f)) {
    num2 = gradient;
  }
  else {
    num2 = (std::exp(num * gradient) - 1.f) / (std::exp(num) - 1.f);
  }
  return (num2 * std::sin(((Math::PI2 * num3) + Math::PI_2) * gradient));
}

} // end of namespace BABYLON
