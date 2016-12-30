#include <babylon/animations/easing/quartic_ease.h>

namespace BABYLON {

QuarticEase::QuarticEase() : EasingFunction{}
{
}

QuarticEase::~QuarticEase()
{
}

float QuarticEase::easeInCore(float gradient) const
{
  return (gradient * gradient * gradient * gradient);
}

} // end of namespace BABYLON
