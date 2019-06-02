#include <babylon/misc/event_state.h>

namespace BABYLON {

EventState::EventState(int iMmask, bool iSkipNextObservers, any* iTarget,
                       any* iCurrentTarget)
{
  initalize(iMmask, iSkipNextObservers, iTarget, iCurrentTarget);
}

EventState::EventState(const EventState& other)
    : skipNextObservers{other.skipNextObservers}
    , mask{other.mask}
    , target{other.target}
    , currentTarget{other.currentTarget}
    , lastReturnValue{other.lastReturnValue}
{
}

EventState::EventState(EventState&& other)
    : skipNextObservers{std::move(other.skipNextObservers)}
    , mask{std::move(other.mask)}
    , target{std::move(other.target)}
    , currentTarget{std::move(other.currentTarget)}
    , lastReturnValue{std::move(other.lastReturnValue)}
{
}

EventState& EventState::operator=(const EventState& other)
{
  if (&other != this) {
    skipNextObservers = other.skipNextObservers;
    mask              = other.mask;
    target            = other.target;
    currentTarget     = other.currentTarget;
    lastReturnValue   = other.lastReturnValue;
  }

  return *this;
}

EventState& EventState::operator=(EventState&& other)
{
  if (&other != this) {
    skipNextObservers = std::move(other.skipNextObservers);
    mask              = std::move(other.mask);
    target            = std::move(other.target);
    currentTarget     = std::move(other.currentTarget);
    lastReturnValue   = std::move(other.lastReturnValue);
  }

  return *this;
}

EventState::~EventState()
{
}

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
