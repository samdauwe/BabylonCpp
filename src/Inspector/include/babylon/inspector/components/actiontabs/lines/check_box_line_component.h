#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_CHECK_BOX_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_CHECK_BOX_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT CheckBoxLineComponent {

  static void render(const char* label, bool& isSelected)
  {
    ImGui::Checkbox(label, &isSelected);
  }

  static void render(const char* label, bool isSelected,
                     const SA::delegate<void(bool value)>& onSelect)
  {
    bool origValue = isSelected;
    ImGui::Checkbox(label, &isSelected);
    if (origValue != isSelected) {
      onSelect(isSelected);
    }
  }

}; // end of struct CheckBoxLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_CHECK_BOX_LINE_COMPONENT_H
