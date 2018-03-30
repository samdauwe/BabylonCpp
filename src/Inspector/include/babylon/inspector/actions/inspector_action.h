#ifndef BABYLON_INSPECTOR_ACTIONS_INSPECTOR_ACTION_H
#define BABYLON_INSPECTOR_ACTIONS_INSPECTOR_ACTION_H

#include <babylon/babylon_stl.h>

namespace BABYLON {

struct InspectorAction {

  InspectorAction();
  InspectorAction(const char* id, const char* icon, const char* label,
                  const char* shortcut, const SA::delegate<void()>& callback);
  InspectorAction(const InspectorAction& rhs);
  InspectorAction(InspectorAction&& rhs);
  InspectorAction& operator=(const InspectorAction& rhs);
  InspectorAction& operator=(InspectorAction&& rhs);
  ~InspectorAction();

  static bool FalseConst();
  void invoke();

  const char* id;
  const char* icon;
  const char* label;
  string_t iconWithLabel;
  const char* shortcut;
  SA::delegate<void()> callback;
  SA::delegate<bool()> isSelected;

}; // end of struct InspectorAction

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ACTIONS_INSPECTOR_ACTION_H
