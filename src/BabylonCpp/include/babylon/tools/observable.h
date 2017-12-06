#ifndef BABYLON_TOOLS_OBSERVABLE_H
#define BABYLON_TOOLS_OBSERVABLE_H

#include <babylon/babylon_global.h>
#include <babylon/tools/event_state.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief The Observable class is a simple implementation of the Observable
 * pattern.
 * There's one slight particularity though: a given Observable can notify
 * its observer using a particular mask value, only the Observers registered
 * with this mask value will be notified.
 * This enable a more fine grained execution without having to rely on
 * multiple different Observable objects.
 * For instance you may have a given Observable that have four different
 * types of notifications: Move (mask = 0x01), Stop (mask = 0x02), Turn Right
 * (mask = 0X04), Turn Left (mask = 0X08).
 * A given observer can register itself with only Move and Stop (mask =
 * 0x03), then it will only be notified when one of these two occurs and will
 * never be for Turn Left/Right.
 */
template <class T>
class Observable {

public:
  using CallbackFunc
    = ::std::function<void(T* eventData, EventState& eventState)>;
  using SPtr = shared_ptr_t<Observable<T>>;

public:
  Observable() : Observable{nullptr}
  {
  }

  Observable(const ::std::function<void(typename Observer<T>::Ptr& observer)>&
               onObserverAdded)
      : _eventState{0}, _onObserverAdded{onObserverAdded}
  {
  }

  Observable& operator=(const Observable&) = delete;

  virtual ~Observable()
  {
  }

  /**
   * @brief Create a new Observer with the specified callback.
   * @param callback the callback that will be executed for that
   * Observer
   * @param mask the mask used to filter observers
   * @param insertFirst if true the callback will be inserted at the
   * first position, hence executed before the others ones. If false (default
   * behavior) the callback will be inserted at the last position, executed
   * after all the others already present.
   * @param scope optional scope for the callback to be called from.
   */
  typename Observer<T>::Ptr add(const CallbackFunc& callback, int mask = -1,
                                bool insertFirst = false, any* scope = nullptr)
  {
    if (!callback) {
      return nullptr;
    }

    auto observer = std::make_shared<Observer<T>>(callback, mask, scope);

    if (insertFirst) {
      _observers.insert(_observers.begin(), observer);
    }
    else {
      _observers.emplace_back(observer);
    }

    if (_onObserverAdded) {
      _onObserverAdded(observer);
    }

    return observer;
  }

  /**
   * @brief Create a new Observer with the specified callback.
   * @param callback the callback that will be executed for that
   * Observer
   * @param mask the mask used to filter observers
   * @param insertFirst if true the callback will be inserted at the
   * first position, hence executed before the others ones. If false (default
   * behavior) the callback will be inserted at the last position, executed
   * after all the others already present.
   */
  typename Observer<T>::Ptr add(CallbackFunc&& callback, int mask = -1,
                                bool insertFirst = false, any* scope = nullptr)
  {
    if (!callback) {
      return nullptr;
    }

    auto observer
      = std::make_shared<Observer<T>>(std::move(callback), mask, scope);

    if (insertFirst) {
      _observers.insert(_observers.begin(), observer);
    }
    else {
      _observers.emplace_back(observer);
    }

    return observer;
  }

  /**
   * @brief Remove an Observer from the Observable object.
   * @param observer the instance of the Observer to remove. If it doesn't
   * belong to this Observable, false will be returned.
   */
  bool remove(typename Observer<T>::Ptr& observer)
  {
    if (!observer) {
      return false;
    }

    auto it
      = ::std::remove_if(_observers.begin(), _observers.end(),
                         [observer](const typename Observer<T>::Ptr& obs) {
                           return obs == observer;
                         });
    if (it != _observers.end()) {
      _observers.erase(it);
      return true;
    }
    return false;
  }

  /**
   * @brief Remove a callback from the Observable object.
   * @param callback the callback to remove. If it doesn't belong to
   * Observable, false will be returned.
   */
  bool removeCallback(const CallbackFunc& callback)
  {
    auto it
      = ::std::remove_if(_observers.begin(), _observers.end(),
                         [callback](const typename Observer<T>::Ptr& obs) {
                           auto ptr1
                             = obs->callback.template target<CallbackFunc>();
                           auto ptr2 = callback.template target<CallbackFunc>();
                           return ptr1 < ptr2;
                         });
    if (it != _observers.end()) {
      _observers.erase(it);
      return true;
    }
    return false;
  }

  /**
   * @brief Notify all Observers by calling their respective callback with the
   * given data.
   * @param eventData
   * @param mask
   */
  bool notifyObservers(T* eventData = nullptr, int mask = -1,
                       any* target = nullptr, any* currentTarget = nullptr)
  {
    if (_observers.empty()) {
      return true;
    }

    auto& state             = _eventState;
    state.mask              = mask;
    state.target            = target;
    state.currentTarget     = currentTarget;
    state.skipNextObservers = false;

    for (auto& obs : _observers) {
      if (obs->mask & mask) {
        obs->callback(eventData, state);
      }
      if (state.skipNextObservers) {
        return false;
      }
    }
    return true;
  }

  /**
   * @brief Notify a specific observer
   * @param eventData
   * @param mask
   */
  void notifyObserver(typename Observer<T>::Ptr& observer,
                      T* eventData = nullptr, int mask = -1)
  {
    auto& state             = _eventState;
    state.mask              = mask;
    state.skipNextObservers = false;

    observer.callback(eventData, state);
  }

  /**
   * @brief Return true is the Observable has at least one Observer registered.
   */
  bool hasObservers() const
  {
    return !_observers.empty();
  }

  /**
   * @brief Clear the list of observers.
   */
  void clear()
  {
    _observers.clear();
    _onObserverAdded = nullptr;
  }

  /**
   * @brief Clone the current observable.
   */
  shared_ptr_t<Observable<T>> clone() const
  {
    Observable<T>::SPtr result = std::make_shared<Observable<T>>();

    result->_observers = _observers;

    return result;
  }

  /**
   * @brief Does this observable handles observer registered with a given mask.
   * @param {number} trigger - the mask to be tested
   * @returns Whether or not one observer registered with the given mask is
   *          handeled.
   **/
  bool hasSpecificMask(int mask = -1)
  {
    for (const auto& obs : _observers) {
      if (obs->mask & mask || obs->mask == mask) {
        return true;
      }
    }
    return false;
  }

private:
  vector_t<typename Observer<T>::Ptr> _observers;
  EventState _eventState;
  ::std::function<void(typename Observer<T>::Ptr& observer)> _onObserverAdded;

}; // end of class Observable

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OBSERVABLE_H
