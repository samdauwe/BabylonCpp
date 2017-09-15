#ifndef BABYLON_TOOLS_OBSERVER_H
#define BABYLON_TOOLS_OBSERVER_H

#include <babylon/babylon_global.h>
#include <babylon/core/fast_func.h>
#include <babylon/tools/event_state.h>

namespace BABYLON {

/**
 * @brief Represent an Observer registered to a given Observable object.
 */
template <class T>
class BABYLON_SHARED_EXPORT Observer {

public:
  using CallbackFunc = FastFunc<void(T* eventData, EventState eventState)>;
  using Ptr          = shared_ptr_t<Observer<T>>;

public:
  Observer() : callback{nullptr}, mask{-1}
  {
  }

  Observer(const CallbackFunc& _callback, int _mask)
      : callback{_callback}, mask{_mask}
  {
  }

  ~Observer()
  {
  }

  operator bool() const
  {
    return (callback == nullptr) && (mask == -1);
  }

public:
  CallbackFunc callback;
  int mask;

}; // end of class Observer

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OBSERVER_H
