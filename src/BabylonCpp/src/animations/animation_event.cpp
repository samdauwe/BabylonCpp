#include <babylon/animations/animation_event.h>

namespace BABYLON {

AnimationEvent::AnimationEvent(
  float iFrame, const std::function<void(float currentFrame)>& iAction,
  bool iOnlyOnce)
    : frame{iFrame}, action{iAction}, onlyOnce{iOnlyOnce}, isDone{false}
{
}

AnimationEvent::AnimationEvent(const AnimationEvent& other) = default;

AnimationEvent::AnimationEvent(AnimationEvent&& other) = default;

AnimationEvent& AnimationEvent::operator=(const AnimationEvent& other) =  default;

AnimationEvent& AnimationEvent::operator=(AnimationEvent&& other) = default;

AnimationEvent::~AnimationEvent() = default;

AnimationEvent AnimationEvent::_clone() const
{
  return AnimationEvent(frame, action, onlyOnce);
}

} // end of namespace BABYLON
