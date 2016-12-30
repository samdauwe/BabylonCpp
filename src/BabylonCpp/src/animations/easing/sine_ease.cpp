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
  return (1.f - std::sin(1.5707963267948966f * (1.f - gradient)));
}

} // end of namespace BABYLON
