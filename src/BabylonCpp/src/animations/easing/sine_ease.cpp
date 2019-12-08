#include <babylon/animations/easing/sine_ease.h>

#include <cmath>

#include <babylon/babylon_constants.h>

namespace BABYLON {

SineEase::~SineEase() = default;

float SineEase::easeInCore(float gradient) const
{
  return (1.f - std::sin(Math::PI_2 * (1.f - gradient)));
}

} // end of namespace BABYLON
