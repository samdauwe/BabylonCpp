#ifndef BABYLON_ANIMATIONS_ANIMATION_EVENT_H
#define BABYLON_ANIMATIONS_ANIMATION_EVENT_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Composed of a frame, and an action function.
 */
class BABYLON_SHARED_EXPORT AnimationEvent {

public:
  /**
   * @brief Initializes the animation event.
   * @param frame The frame for which the event is triggered
   * @param action The event to perform when triggered
   * @param onlyOnce Specifies if the event should be triggered only once
   */
  AnimationEvent(int frame,
                 const ::std::function<void(float currentFrame)>& action,
                 bool onlyOnce = false);
  AnimationEvent(const AnimationEvent& other);
  AnimationEvent(AnimationEvent&& other);
  AnimationEvent& operator=(const AnimationEvent& other);
  AnimationEvent& operator=(AnimationEvent&& other);
  ~AnimationEvent();

  /**
   * @brief Hidden
   */
  AnimationEvent _clone() const;

public:
  /**
   * The frame for which the event is triggered
   */
  int frame;

  /**
   * The event to perform when triggered
   */
  ::std::function<void(float currentFrame)> action;

  /**
   * Specifies if the event should be triggered only once
   */
  bool onlyOnce;

  /**
   * Specifies if the animation event is done
   */
  bool isDone;

}; // end of class AnimationEvent

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_EVENT_H
