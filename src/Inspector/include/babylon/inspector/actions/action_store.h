#ifndef BABYLON_INSPECTOR_ACTIONS_ACTION_STORE_H
#define BABYLON_INSPECTOR_ACTIONS_ACTION_STORE_H

#include <string>
#include <unordered_map>

#include <babylon/inspector/actions/inspector_action.h>

namespace BABYLON {

class ActionStore {

public:
  ActionStore();
  ~ActionStore();

  void addAction(const char* id, const char* icon, const char* label,
                 const char* shortcut, const SA::delegate<void()>& callback);
  InspectorAction* getAction(const char* id);

private:
  std::unordered_map<std::string, InspectorAction> _actions;

}; // end of class EditorStore

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ACTIONS_ACTION_STORE_H
