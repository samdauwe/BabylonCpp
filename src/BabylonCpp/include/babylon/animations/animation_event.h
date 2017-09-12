#ifndef BABYLON_ANIMATIONS_ANIMATION_EVENT_H
#define BABYLON_ANIMATIONS_ANIMATION_EVENT_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Composed of a frame, and an action function.
 */
class BABYLON_SHARED_EXPORT AnimationEvent {

public:
  AnimationEvent(int frame, const ::std::function<void()>& action,
                 bool onlyOnce = false);
  ~AnimationEvent();

public:
  int frame;
  ::std::function<void()> action;
  bool onlyOnce;
  bool isDone;

}; // end of class AnimationEvent

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_EVENT_H
