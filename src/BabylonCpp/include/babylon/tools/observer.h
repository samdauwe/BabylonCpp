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
  /**
   * @brief Creates a new observer.
   */
  Observer()
      : _willBeUnregistered{false}
      , unregisterOnNextCall{false}
      , callback{nullptr}
      , mask{-1}
      , scope{nullptr}
  {
  }

  /**
   * @brief Creates a new observer.
   * @param callback defines the callback to call when the observer is notified
   * @param mask defines the mask of the observer (used to filter notifications)
   * @param scope defines the current scope used to restore the JS context
   */
  Observer(const CallbackFunc& iCallback, int iMask, any* iScope)
      : _willBeUnregistered{false}
      , unregisterOnNextCall{false}
      , callback{iCallback}
      , mask{iMask}
      , scope{iScope}
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
  /** Hidden */
  bool _willBeUnregistered;
  /**
   * Gets or sets a property defining that the observer as to be unregistered
   * after the next notification
   */
  bool unregisterOnNextCall;
  /**
   * Defines the callback to call when the observer is notified
   */
  CallbackFunc callback;
  /**
   * Defines the mask of the observer (used to filter notifications)
   */
  int mask;
  /**
   * Defines the current scope used to restore the JS context
   */
  any* scope;

}; // end of class Observer

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OBSERVER_H
