#include <babylon/animations/easing/back_ease.h>

namespace BABYLON {

BackEase::BackEase(float iAmplitude) : EasingFunction{}, amplitude{iAmplitude}
{
}

BackEase::~BackEase()
{
}

float BackEase::easeInCore(float gradient) const
{
  float num = ::std::max(0.f, amplitude);
  return (::std::pow(gradient, 3.f)
          - ((gradient * num) * ::std::sin(Math::PI * gradient)));
}

} // end of namespace BABYLON
