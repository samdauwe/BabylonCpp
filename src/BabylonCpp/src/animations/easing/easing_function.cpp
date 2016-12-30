#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

EasingFunction::EasingFunction()
    : _easingMode{EasingFunction::EASINGMODE_EASEIN}
{
}

EasingFunction::~EasingFunction()
{
}

void EasingFunction::setEasingMode(int easingMode)
{
  _easingMode = std::min(std::max(easingMode, 0), 2);
}

int EasingFunction::getEasingMode() const
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
    return (((1 - easeInCore((1 - gradient) * 2)) * 0.5f) + 0.5f);
  }

  return (easeInCore(gradient * 2) * 0.5f);
}

} // end of namespace BABYLON
