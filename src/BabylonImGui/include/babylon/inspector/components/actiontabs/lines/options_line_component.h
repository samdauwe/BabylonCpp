#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_OPTIONS_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_OPTIONS_LINE_COMPONENT_H

#include <algorithm>
#include <optional>
#include <vector>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT OptionsLineComponent {

  static constexpr float Width = 241.f;

  static std::optional<unsigned int>
  render(const char* label, unsigned int currentValue,
         const std::vector<std::pair<const char*, unsigned int>>& options)
  {
    std::optional<unsigned int> result = std::nullopt;

    auto it = std::find_if(
      options.begin(), options.end(),
      [currentValue](const std::pair<const char*, unsigned int>& item) {
        return item.second == currentValue;
      });
    if (it == options.end()) {
      return result;
    }
    auto currentIndex = static_cast<size_t>(it - options.begin());

    ImGui::TextWrapped("%s", label);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - OptionsLineComponent::Width);
    ImGui::PushID(label);
    ImGui::PushItemWidth(OptionsLineComponent::Width);
    if (ImGui::BeginCombo("", options[currentIndex].first)) {
      for (size_t n = 0; n < options.size(); ++n) {
        bool isSelected = (currentIndex == n);
        if (ImGui::Selectable(options[n].first, isSelected)) {
          result = options[n].second;
        }
        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    ImGui::PopID();

    return result;
  }

}; // end of struct OptionsLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_OPTIONS_LINE_COMPONENT_H
