#include <babylon/animations/easing/quintic_ease.h>

namespace BABYLON {

QuinticEase::QuinticEase() : EasingFunction{}
{
}

QuinticEase::~QuinticEase()
{
}

float QuinticEase::easeInCore(float gradient) const
{
  return (gradient * gradient * gradient * gradient * gradient);
}

} // end of namespace BABYLON
