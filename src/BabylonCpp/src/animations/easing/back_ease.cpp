#include <babylon/animations/easing/back_ease.h>

namespace BABYLON {

BackEase::BackEase(float amplitude) : EasingFunction{}, _amplitude{amplitude}
{
}

BackEase::~BackEase()
{
}

float BackEase::easeInCore(float gradient) const
{
  float num = std::fmax(0.f, _amplitude);
  return (std::pow(gradient, 3.f)
          - ((gradient * num) * std::sin(Math::PI * gradient)));
}

} // end of namespace BABYLON
