#ifndef BABYLON_MISC_OBSERVABLE_H
#define BABYLON_MISC_OBSERVABLE_H

#include <babylon/babylon_api.h>
#include <babylon/misc/event_state.h>
#include <babylon/misc/observer.h>

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
class BABYLON_SHARED_EXPORT Observable {

public:
  using CallbackFunc
    = std::function<void(T* eventData, EventState& eventState)>;
  using SPtr = std::shared_ptr<Observable<T>>;

public:
  /**
   * @brief Creates a new observable.
   */
  Observable() : Observable{nullptr}
  {
  }

  /**
   * @brief Creates a new observable.
   * @param onObserverAdded defines a callback to call when a new observer is
   * added
   */
  Observable(const std::function<void(typename Observer<T>::Ptr& observer)>&
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
   * @param callback the callback that will be executed for that Observer
   * @param mask the mask used to filter observers
   * @param insertFirst if true the callback will be inserted at the first
   * position, hence executed before the others ones. If false (default
   * behavior) the callback will be inserted at the last position, executed
   * after all the others already present.
   * @param scope optional scope for the callback to be called from
   * @param unregisterOnFirstCall defines if the observer as to be unregistered
   * after the next notification
   * @returns the new observer created for the callback
   */
  typename Observer<T>::Ptr add(const CallbackFunc& callback, int mask = -1,
                                bool insertFirst = false, any* scope = nullptr,
                                bool unregisterOnFirstCall = false)
  {
    if (!callback) {
      return nullptr;
    }

    auto observer = std::make_shared<Observer<T>>(callback, mask, scope);
    observer->unregisterOnNextCall = unregisterOnFirstCall;

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
   * @param callback the callback that will be executed for that Observer
   * @param mask the mask used to filter observers
   * @param insertFirst if true the callback will be inserted at the first
   * position, hence executed before the others ones. If false (default
   * behavior) the callback will be inserted at the last position, executed
   * after all the others already present.
   * @param scope optional scope for the callback to be called from
   * @param unregisterOnFirstCall defines if the observer as to be unregistered
   * after the next notification
   * @returns the new observer created for the callback
   */
  typename Observer<T>::Ptr add(CallbackFunc&& callback, int mask = -1,
                                bool insertFirst = false, any* scope = nullptr,
                                bool unregisterOnFirstCall = false)
  {
    if (!callback) {
      return nullptr;
    }

    auto observer
      = std::make_shared<Observer<T>>(std::move(callback), mask, scope);
    observer->unregisterOnNextCall = unregisterOnFirstCall;

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
   * @brief Create a new Observer with the specified callback and unregisters
   * after the next notification.
   * @param callback the callback that will be executed for that Observer
   * @returns the new observer created for the callback
   */
  typename Observer<T>::Ptr addOnce(CallbackFunc&& callback)
  {
    return add(callback, -1, false, nullptr, true);
  }

  /**
   * @brief Remove an Observer from the Observable object.
   * @param observer the instance of the Observer to remove
   * @returns false if it doesn't belong to this Observable
   */
  bool remove(typename Observer<T>::Ptr& observer)
  {
    if (!observer) {
      return false;
    }

    auto it = std::find_if(_observers.begin(), _observers.end(),
                           [observer](const typename Observer<T>::Ptr& obs) {
                             return obs == observer;
                           });

    if (it != _observers.end()) {
      _deferUnregister(*it);
      return true;
    }
    return false;
  }

  /**
   * @brief Remove a callback from the Observable object.
   * @param callback the callback to remove
   * @param scope optional scope. If used only the callbacks with this scope
   * will be removed
   * @returns false if it doesn't belong to this Observable
   */
  bool removeCallback(const CallbackFunc& callback)
  {
    auto it = std::find_if(_observers.begin(), _observers.end(),
                           [callback](const typename Observer<T>::Ptr& obs) {
                             auto ptr1
                               = obs->callback.template target<CallbackFunc>();
                             auto ptr2
                               = callback.template target<CallbackFunc>();
                             return ptr1 < ptr2;
                           });

    if (it != _observers.end()) {
      _deferUnregister(*it);
      return true;
    }
    return false;
  }

private:
  void _deferUnregister(typename Observer<T>::Ptr& observer)
  {
    observer->unregisterOnNextCall = false;
    observer->_willBeUnregistered  = true;
    _remove(observer);
  }

  // This should only be called when not iterating over _observers to avoid
  // callback skipping. Removes an observer from the _observer Array.
  bool _remove(typename Observer<T>::Ptr& observer)
  {
    if (!observer) {
      return false;
    }

    auto index = std::find(_observers.begin(), _observers.end(), observer);

    if (index != _observers.end()) {
      _observers.erase(index);
      return true;
    }

    return false;
  }

public:
  /**
   * @brief Moves the observable to the top of the observer list making it get
   * called first when notified.
   * @param observer the observer to move
   */
  void makeObserverTopPriority(typename Observer<T>::Ptr& observer)
  {
    _remove(observer);
    _observers.insert(_observers.begin(), observer);
  }

  /**
   * @brief Moves the observable to the bottom of the observer list making it
   * get called last when notified.
   * @param observer the observer to move
   */
  void makeObserverBottomPriority(typename Observer<T>::Ptr& observer)
  {
    _remove(observer);
    _observers.emplace_back(observer);
  }

  /**
   * @brief Notify all Observers by calling their respective callback with the
   * given data. Will return true if all observers were executed, false if an
   * observer set skipNextObservers to true, then prevent the subsequent ones to
   * execute
   * @param eventData defines the data to send to all observers
   * @param mask defines the mask of the current notification (observers with
   * incompatible mask (ie mask & observer.mask === 0) will not be notified)
   * @param target defines the original target of the state
   * @param currentTarget defines the current target of the state
   * @returns false if the complete observer chain was not processed (because
   * one observer set the skipNextObservers to true)
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
    state.lastReturnValue   = eventData;

    for (auto& obs : _observers) {
      if (obs->_willBeUnregistered) {
        continue;
      }

      if (obs->mask & mask) {
        obs->callback(eventData, state);

        if (obs->unregisterOnNextCall) {
          _deferUnregister(obs);
        }
      }
      if (state.skipNextObservers) {
        return false;
      }
    }
    return true;
  }

  /**
   * @brief Notify a specific observer.
   * @param observer defines the observer to notify
   * @param eventData defines the data to be sent to each callback
   * @param mask is used to filter observers defaults to -1
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
   * @brief Gets a boolean indicating if the observable has at least one
   * observer.
   * @returns true is the Observable has at least one Observer registered
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
   * @returns a new observable
   */
  std::shared_ptr<Observable<T>> clone() const
  {
    Observable<T>::SPtr result = std::make_shared<Observable<T>>();

    result->_observers = _observers;

    return result;
  }

  /**
   * @brief Does this observable handles observer registered with a given mask.
   * @param mask defines the mask to be tested
   * @return whether or not one observer registered with the given mask is
   *handeled
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
  std::vector<typename Observer<T>::Ptr> _observers;
  EventState _eventState;
  std::function<void(typename Observer<T>::Ptr& observer)> _onObserverAdded;

}; // end of class Observable

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OBSERVABLE_H
