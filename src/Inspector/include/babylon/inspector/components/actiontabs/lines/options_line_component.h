#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_OPTIONS_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_OPTIONS_LINE_COMPONENT_H

#include <algorithm>
#include <vector>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT OptionsLineComponent {

  static void
  render(const char* label, unsigned int currentValue,
         const std::vector<std::pair<const char*, unsigned int>>& options,
         const SA::delegate<void(unsigned int)> onSelect)
  {
    auto it = std::find_if(
      options.begin(), options.end(),
      [currentValue](const std::pair<const char*, unsigned int>& item) {
        return item.second == currentValue;
      });
    if (it == options.end()) {
      return;
    }
    auto currentIndex = static_cast<size_t>(it - options.begin());

    if (ImGui::BeginCombo(label, options[currentIndex].first)) {
      for (size_t n = 0; n < options.size(); ++n) {
        bool isSelected = (currentIndex == n);
        if (ImGui::Selectable(options[n].first, isSelected)) {
          currentValue = options[n].second;
          onSelect(currentValue);
        }
        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
  }

}; // end of struct OptionsLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_OPTIONS_LINE_COMPONENT_H
