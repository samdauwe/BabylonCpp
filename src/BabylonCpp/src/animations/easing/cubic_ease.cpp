#include <babylon/animations/easing/cubic_ease.h>

namespace BABYLON {

CubicEase::CubicEase() : EasingFunction{}
{
}

CubicEase::~CubicEase()
{
}

float CubicEase::easeInCore(float gradient) const
{
  return (gradient * gradient * gradient);
}

} // end of namespace BABYLON
