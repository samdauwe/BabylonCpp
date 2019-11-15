#include <babylon/misc/event_state.h>

namespace BABYLON {

EventState::EventState(int iMmask, bool iSkipNextObservers, any* iTarget,
                       any* iCurrentTarget)
{
  initalize(iMmask, iSkipNextObservers, iTarget, iCurrentTarget);
}

EventState::EventState(const EventState& other) = default;

EventState::EventState(EventState&& other) = default;

EventState& EventState::operator=(const EventState& other) = default;

EventState& EventState::operator=(EventState&& other) = default;

EventState::~EventState() = default;

EventState& EventState::initalize(int iMmask, bool iSkipNextObservers,
                                  any* iTarget, any* iCurrentTarget)
{
  mask              = iMmask;
  skipNextObservers = iSkipNextObservers;
  target            = iTarget;
  currentTarget     = iCurrentTarget;
  lastReturnValue   = nullptr;
  return *this;
}

} // end of namespace BABYLON
