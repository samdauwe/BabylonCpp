#ifndef BABYLON_ANIMATIONS_EASING_IEASING_FUNCTION_H
#define BABYLON_ANIMATIONS_EASING_IEASING_FUNCTION_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT IEasingFunction {

public:
  virtual float ease(float gradient) = 0;

}; // end of class IEasingFunction

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_IEASING_FUNCTION_H
