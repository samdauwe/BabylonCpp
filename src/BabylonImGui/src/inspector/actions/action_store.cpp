#include <babylon/inspector/actions/action_store.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

ActionStore::ActionStore() =  default;

ActionStore::~ActionStore() = default;

void ActionStore::addAction(const char* id, const char* icon, const char* label,
                            const char* shortcut,
                            const SA::delegate<void()>& callback)
{
  _actions[id] = InspectorAction(id, icon, label, shortcut, callback);
}

InspectorAction* ActionStore::getAction(const char* id)
{
  if (stl_util::contains(_actions, id)) {
    return &_actions[id];
  }

  return nullptr;
}

} // end of namespace BABYLON
