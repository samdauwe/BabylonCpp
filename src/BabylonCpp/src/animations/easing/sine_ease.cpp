#include <babylon/animations/easing/sine_ease.h>

namespace BABYLON {

SineEase::SineEase() : EasingFunction{}
{
}

SineEase::~SineEase()
{
}

float SineEase::easeInCore(float gradient) const
{
  return (1.f - ::std::sin(Math::PI_2 * (1.f - gradient)));
}

} // end of namespace BABYLON
