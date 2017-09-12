#include <babylon/animations/animation_event.h>

namespace BABYLON {

AnimationEvent::AnimationEvent(int iFrame,
                               const ::std::function<void()>& iAction,
                               bool iOnlyOnce)
    : frame{iFrame}, action{iAction}, onlyOnce{iOnlyOnce}, isDone{false}
{
}

AnimationEvent::~AnimationEvent()
{
}

} // end of namespace BABYLON
