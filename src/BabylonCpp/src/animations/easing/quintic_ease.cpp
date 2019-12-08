#include <babylon/animations/easing/quintic_ease.h>

namespace BABYLON {

QuinticEase::~QuinticEase() = default;

float QuinticEase::easeInCore(float gradient) const
{
  return (gradient * gradient * gradient * gradient * gradient);
}

} // end of namespace BABYLON
