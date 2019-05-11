#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_CHECK_BOX_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_CHECK_BOX_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/imgui/imgui_utils.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT CheckBoxLineComponent {

  /**
   * @brief Renders a checkbox widget.
   * @param label checkbox label
   * @param isSelected if the checbox is selected
   * @return whether or not the checbox value was changed
   */
  static bool render(const char* label, bool isSelected)
  {
    bool origValue = isSelected;
    ImGui::TextWrapped("%s", label);
    auto width = ImGui::GetFrameHeight() * 1.55f;
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - width);
    ImGui::PushID(label);
    ImGui::SwitchButton("", &isSelected);
    ImGui::PopID();
    return origValue != isSelected;
  }

}; // end of struct CheckBoxLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_CHECK_BOX_LINE_COMPONENT_H
