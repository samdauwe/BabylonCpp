#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VALUE_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VALUE_LINE_COMPONENT_H

#include <optional>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ValueLineComponent {

  static constexpr float CharacterWidth = 7.f;

  static void render(const char* label, int value,
                     const std::optional<ImVec4>& color = std::nullopt,
                     const char* units                  = "")
  {
    ImGui::TextWrapped("%s", label);
    if (color.has_value()) {
      ImGui::PushStyleColor(ImGuiCol_Text, *color);
    }
    char valueStr[16] = {};
    sprintf(valueStr, "%d %s", value, units);
    const auto strLen = strlen(valueStr);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - strLen * ValueLineComponent::CharacterWidth);
    ImGui::TextWrapped("%s", valueStr);
    if (color.has_value()) {
      ImGui::PopStyleColor();
    }
  }

  static void render(const char* label, float value,
                     const std::optional<ImVec4>& color       = std::nullopt,
                     const std::optional<int>& fractionDigits = std::nullopt,
                     const char* units                        = "")
  {
    const auto digits = fractionDigits.has_value() ? *fractionDigits : 2;

    ImGui::TextWrapped("%s", label);
    if (color.has_value()) {
      ImGui::PushStyleColor(ImGuiCol_Text, *color);
    }
    char valueStr[16] = {};
    sprintf(valueStr, "%.*f %s", digits, static_cast<double>(value), units);
    const auto strLen = strlen(valueStr) + 1;
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - strLen * ValueLineComponent::CharacterWidth);
    ImGui::TextWrapped("%s", valueStr);
    if (color.has_value()) {
      ImGui::PopStyleColor();
    }
  }

}; // end of struct ValueLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VALUE_LINE_COMPONENT_H
