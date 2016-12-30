#include <babylon/animations/easing/elastic_ease.h>

namespace BABYLON {

ElasticEase::ElasticEase(int oscillations, int springiness)
    : EasingFunction{},
      _oscillations{static_cast<float>(oscillations)},
      _springiness{static_cast<float>(springiness)}
{
}

ElasticEase::~ElasticEase()
{
}

float ElasticEase::easeInCore(float gradient) const
{
  float num2 = 0.f;
  float num3 = std::fmax(0.f, _oscillations);
  float num  = std::fmax(0.f, _springiness);

  if (std_util::almost_equal(num, 0.f)) {
    num2 = gradient;
  }
  else {
    num2 = (std::exp(num * gradient) - 1.f) / (std::exp(num) - 1.f);
  }
  return (num2 * std::sin(((6.2831853071795862f * num3) + 1.5707963267948966f)
                          * gradient));
}

} // end of namespace BABYLON
