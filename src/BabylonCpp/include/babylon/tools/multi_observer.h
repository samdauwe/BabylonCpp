#ifndef BABYLON_TOOLS_MULTI_OBSERVER_H
#define BABYLON_TOOLS_MULTI_OBSERVER_H

#include <babylon/babylon_api.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief Represent a list of observers registered to multiple Observables
 * object.
 */
template <class T>
class BABYLON_SHARED_EXPORT MultiObserver {

public:
  using CallbackFunc
    = std::function<void(T* eventData, EventState& eventState)>;
  using SPtr = std::shared_ptr<MultiObserver<T>>;

public:
  MultiObserver()
  {
  }

  ~MultiObserver()
  {
  }

  /**
   * @brief Release associated resources.
   */
  void dispose()
  {
    if (!_observers.empty() && !_observables.empty()) {
      for (size_t index = 0; index < _observers.size(); ++index) {
        _observables[index].remove(_observers[index]);
      }
    }

    _observers.clear();
    _observables.clear();
  }

  /**
   * @brief Raise a callback when one of the observable will notify.
   * @param observables defines a list of observables to watch
   * @param callback defines the callback to call on notification
   * @param mask defines the mask used to filter notifications
   * @param scope defines the current scope used to restore the JS context
   * @returns the new MultiObserver
   */
  static std::shared_ptr<MultiObserver<T>>
  Watch(const std::vector<typename Observable<T>::Ptr>& observables,
        const CallbackFunc& callback, int mask = -1, any* scope = nullptr)
  {
    auto result = std::make_shared<MultiObserver<T>>();

    result._observers.clear();
    result._observables = observables;

    for (const auto& observable : observables) {
      auto observer = observable.add(callback, mask, false, scope);
      if (observer) {
        result._observers.emplace_back(observer);
      }
    }

    return result;
  }

private:
  std::vector<typename Observer<T>::Ptr> _observers;
  std::vector<typename Observable<T>::Ptr> _observables;

}; // end of class MultiObserver<T>

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_MULTI_OBSERVER_H
