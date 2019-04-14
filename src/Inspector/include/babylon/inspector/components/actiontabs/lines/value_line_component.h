#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VALUE_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VALUE_LINE_COMPONENT_H

#include <optional>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ValueLineComponent {

  static void render(const char* label, int value,
                     const std::optional<ImVec4>& color = std::nullopt,
                     const std::string& units           = "")
  {
    ImGui::TextWrapped("%s", label);
    ImGui::NextColumn();
    if (color.has_value()) {
      ImGui::PushStyleColor(ImGuiCol_Text, *color);
    }
    ImGui::TextWrapped("%d %s", value, units.c_str());
    if (color.has_value()) {
      ImGui::PopStyleColor();
    }
    ImGui::NextColumn();
  }

  static void render(const char* label, float value,
                     const std::optional<ImVec4>& color       = std::nullopt,
                     const std::optional<int>& fractionDigits = std::nullopt,
                     const std::string& units                 = "")
  {
    const auto digits = fractionDigits.has_value() ? *fractionDigits : 2;

    ImGui::TextWrapped("%s", label);
    ImGui::NextColumn();
    if (color.has_value()) {
      ImGui::PushStyleColor(ImGuiCol_Text, *color);
    }
    ImGui::TextWrapped("%.*f %s", digits, static_cast<double>(value),
                       units.c_str());
    if (color.has_value()) {
      ImGui::PopStyleColor();
    }
    ImGui::NextColumn();
  }

}; // end of struct ValueLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VALUE_LINE_COMPONENT_H
