#ifndef BABYLON_ANIMATIONS_ANIMATION_KEY_H
#define BABYLON_ANIMATIONS_ANIMATION_KEY_H

#include <babylon/babylon_global.h>

#include <babylon/animations/animation_value.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT AnimationKey {

public:
  AnimationKey(int frame, AnimationValue value);

public:
  int frame;
  AnimationValue value;

}; // end of class AnimationKey

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_KEY_H
