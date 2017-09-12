#include <babylon/tools/event_state.h>

namespace BABYLON {

EventState::EventState(int _mask, bool _skipNextObservers)
{
  initalize(_mask, _skipNextObservers);
}

EventState::EventState(const EventState& other)
    : skipNextObservers{other.skipNextObservers}, mask{other.mask}
{
}

EventState::EventState(EventState&& other)
{
  *this = ::std::move(other);
}

EventState& EventState::operator=(const EventState& other)
{
  if (&other != this) {
    skipNextObservers = other.skipNextObservers;
    mask              = other.mask;
  }

  return *this;
}

EventState& EventState::operator=(EventState&& other)
{
  if (&other != this) {
    skipNextObservers = ::std::move(other.skipNextObservers);
    mask              = ::std::move(other.mask);
  }

  return *this;
}

EventState::~EventState()
{
}

EventState& EventState::initalize(int _mask, bool _skipNextObservers)
{
  mask              = _mask;
  skipNextObservers = _skipNextObservers;
  return *this;
}

} // end of namespace BABYLON
