#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_RADIO_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_RADIO_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT RadioButtonLineComponent {

  static constexpr float Width = 19.f;

  /**
   * @brief Renders a radio button widget
   * @param label checkbox label
   * @param isSelected if the checbox is selected
   * @return whether or not the checbox value was seleced
   */
  static bool render(const char* label, bool isSelected)
  {
    ImGui::TextWrapped("%s", label);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - RadioButtonLineComponent::Width);
    ImGui::PushID(label);
    isSelected = ImGui::RadioButton(label, isSelected);
    ImGui::PopID();
    return isSelected;
  }

}; // end of struct RadioButtonLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_RADIO_LINE_COMPONENT_H
