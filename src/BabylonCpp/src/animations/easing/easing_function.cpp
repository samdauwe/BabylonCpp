#include <babylon/animations/easing/easing_function.h>

#include <cmath>

#include <algorithm>

namespace BABYLON {

EasingFunction::EasingFunction()
    : _easingMode{EasingFunction::EASINGMODE_EASEIN}
{
}

EasingFunction::~EasingFunction()
{
}

void EasingFunction::setEasingMode(unsigned int easingMode)
{
  _easingMode = std::min(easingMode, 2u);
}

unsigned int EasingFunction::getEasingMode() const
{
  return _easingMode;
}

float EasingFunction::ease(float gradient)
{
  switch (_easingMode) {
    case EasingFunction::EASINGMODE_EASEIN:
      return easeInCore(gradient);
    case EasingFunction::EASINGMODE_EASEOUT:
      return (1 - easeInCore(1 - gradient));
    default:
      break;
  }

  if (gradient >= 0.5f) {
    return (((1.f - easeInCore((1.f - gradient) * 2.f)) * 0.5f) + 0.5f);
  }

  return (easeInCore(gradient * 2.f) * 0.5f);
}

} // end of namespace BABYLON
