#ifndef BABYLON_INSPECTOR_COMPONENTS_GLOBAL_STATE_H
#define BABYLON_INSPECTOR_COMPONENTS_GLOBAL_STATE_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/entity.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class GlobalState;
using GlobalStatePtr = std::shared_ptr<GlobalState>;

class BABYLON_SHARED_EXPORT GlobalState {

public:
  template <typename... Ts>
  static GlobalStatePtr New(Ts&&... args)
  {
    return std::shared_ptr<GlobalState>(
      new GlobalState(std::forward<Ts>(args)...));
  }
  ~GlobalState() = default;

protected:
  GlobalState() = default;

public:
  Observable<EntityInfo> onSelectionChangedObservable;

}; // end of struct TreeItemSpecializedComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_GLOBAL_STATE_H
