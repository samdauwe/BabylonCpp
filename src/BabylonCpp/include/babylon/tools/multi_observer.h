#ifndef BABYLON_TOOLS_MULTI_OBSERVER_H
#define BABYLON_TOOLS_MULTI_OBSERVER_H

#include <babylon/babylon_global.h>
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
    = ::std::function<void(T* eventData, EventState& eventState)>;
  using SPtr = shared_ptr_t<MultiObserver<T>>;

public:
  MultiObserver()
  {
  }

  ~MultiObserver()
  {
  }

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

  static shared_ptr_t<MultiObserver<T>>
  Watch(const vector_t<typename Observable<T>::Ptr>& observables,
        const CallbackFunc& callback, int mask = -1, any* scope = nullptr)
  {
    MultiObserver<T>::SPtr result = ::std::make_shared<MultiObserver<T>>();

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
  vector_t<typename Observer<T>::Ptr> _observers;
  vector_t<typename Observable<T>::Ptr> _observables;

}; // end of class MultiObserver<T>

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_MULTI_OBSERVER_H
