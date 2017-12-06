#ifndef BABYLON_TOOLS_OBSERVER_H
#define BABYLON_TOOLS_OBSERVER_H

#include <babylon/babylon_global.h>
#include <babylon/tools/event_state.h>

namespace BABYLON {

/**
 * @brief Represent an Observer registered to a given Observable object.
 */
template <class T>
class BABYLON_SHARED_EXPORT Observer {

public:
  using CallbackFunc
    = ::std::function<void(T* eventData, EventState& eventState)>;
  using Ptr = shared_ptr_t<Observer<T>>;

public:
  Observer() : callback{nullptr}, mask{-1}, scope{nullptr}
  {
  }

  Observer(const CallbackFunc& iCallback, int iMask, any* iScope)
      : callback{iCallback}, mask{iMask}, scope{iScope}
  {
  }

  ~Observer()
  {
  }

  operator bool() const
  {
    return (callback == nullptr) && (mask == -1) && (scope == nullptr);
  }

public:
  CallbackFunc callback;
  int mask;
  any* scope;

}; // end of class Observer

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OBSERVER_H
