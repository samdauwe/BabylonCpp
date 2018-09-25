#include <babylon/animations/animation_event.h>

namespace BABYLON {

AnimationEvent::AnimationEvent(
  int iFrame, const std::function<void(float currentFrame)>& iAction,
  bool iOnlyOnce)
    : frame{iFrame}, action{iAction}, onlyOnce{iOnlyOnce}, isDone{false}
{
}

AnimationEvent::AnimationEvent(const AnimationEvent& other)
    : frame{other.frame}
    , action{other.action}
    , onlyOnce{other.onlyOnce}
    , isDone{other.isDone}
{
}

AnimationEvent::AnimationEvent(AnimationEvent&& other)
    : frame{std::move(other.frame)}
    , action{std::move(other.action)}
    , onlyOnce{std::move(other.onlyOnce)}
    , isDone{std::move(other.isDone)}
{
}

AnimationEvent& AnimationEvent::operator=(const AnimationEvent& other)
{
  if (&other != this) {
    frame    = other.frame;
    action   = other.action;
    onlyOnce = other.onlyOnce;
    isDone   = other.isDone;
  }

  return *this;
}

AnimationEvent& AnimationEvent::operator=(AnimationEvent&& other)
{
  if (&other != this) {
    frame    = std::move(other.frame);
    action   = std::move(other.action);
    onlyOnce = std::move(other.onlyOnce);
    isDone   = std::move(other.isDone);
  }

  return *this;
}

AnimationEvent::~AnimationEvent()
{
}

AnimationEvent AnimationEvent::_clone() const
{
  return AnimationEvent(frame, action, onlyOnce);
}

} // end of namespace BABYLON
