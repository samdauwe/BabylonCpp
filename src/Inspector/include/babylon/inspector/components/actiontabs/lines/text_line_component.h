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

  static constexpr float CharacterWidth = 7.f;

  static void renderContent(const char* value,
                            const std::optional<ImVec4>& color = std::nullopt,
                            bool /*underline*/                 = false,
                            const std::function<void()>& /*onLink*/ = nullptr)
  {
    if (color.has_value()) {
      ImGui::PushStyleColor(ImGuiCol_Text, *color);
    }
    const auto strLen = strlen(value) + 1;
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - strLen * TextLineComponent::CharacterWidth);
    ImGui::TextWrapped("%s", (strLen == 0) ? "no name" : value);
    if (color.has_value()) {
      ImGui::PopStyleColor();
    }
  }

  static void render(const char* label, const std::string& value,
                     const std::optional<ImVec4>& color  = std::nullopt,
                     bool underline                      = false,
                     const std::function<void()>& onLink = nullptr)
  {
    ImGui::TextWrapped("%s", label);
    TextLineComponent::renderContent(value.c_str(), color, underline, onLink);
  }

  static void render(const char* text)
  {
    ImGui::TextWrapped("%s", text);
  }

}; // end of struct TextLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXT_LINE_COMPONENT_H
