#include <babylon/animations/easing/quadratic_ease.h>

namespace BABYLON {

QuadraticEase::QuadraticEase() : EasingFunction{}
{
}

QuadraticEase::~QuadraticEase() = default;

float QuadraticEase::easeInCore(float gradient) const
{
  return (gradient * gradient);
}

} // end of namespace BABYLON
