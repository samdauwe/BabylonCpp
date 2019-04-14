#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXT_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXT_LINE_COMPONENT_H

#include <functional>
#include <optional>
#include <string>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TextLineComponent {

  static void renderContent(const std::string& value,
                            const std::optional<ImVec4>& color = std::nullopt,
                            bool /*underline*/                 = false,
                            const std::function<void()>& /*onLink*/ = nullptr)
  {
    if (color.has_value()) {
      ImGui::PushStyleColor(ImGuiCol_Text, *color);
    }
    ImGui::TextWrapped("%s", value.empty() ? "no name" : value.c_str());
    if (color.has_value()) {
      ImGui::PopStyleColor();
    }
  }

  static void render(const std::string& label, const std::string& value,
                     const std::optional<ImVec4>& color  = std::nullopt,
                     bool underline                      = false,
                     const std::function<void()>& onLink = nullptr)
  {
    ImGui::TextWrapped("%s", label.c_str());
    ImGui::NextColumn();
    TextLineComponent::renderContent(value, color, underline, onLink);
    ImGui::NextColumn();
  }

  static void render(const std::string& text)
  {
    ImGui::TextWrapped("%s", text.c_str());
  }

}; // end of struct TextLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXT_LINE_COMPONENT_H
