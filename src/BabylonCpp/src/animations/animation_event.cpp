#include <babylon/animations/animation_event.h>

namespace BABYLON {

AnimationEvent::AnimationEvent(int _frame, std::function<void()>& _action,
                               bool _onlyOnce)
    : frame{_frame}, action{_action}, onlyOnce{_onlyOnce}, isDone{false}
{
}

AnimationEvent::~AnimationEvent()
{
}

} // end of namespace BABYLON
