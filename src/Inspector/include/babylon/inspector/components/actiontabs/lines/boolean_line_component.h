#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BOOLEAN_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BOOLEAN_LINE_COMPONENT_H

#include <string>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome_5.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BooleanLineComponent {

  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faCheck       = ICON_FA_CHECK;
  static constexpr const char* faTimesCircle = ICON_FA_TIMES_CIRCLE;

  static void render(const std::string& label, bool value, float offsetX = 0.f)
  {
    static ImVec4 red   = ImColor(1.0f, 0.0f, 0.0f, 1.0f);
    static ImVec4 green = ImColor(0.0f, 1.0f, 0.0f, 1.0f);

    ImGui::TextWrapped("%s", label.c_str());
    ImGui::SameLine(offsetX);
    ImGui::PushStyleColor(ImGuiCol_Text, value ? green : red);
    ImGui::TextWrapped("%s", value ? faCheck : faTimesCircle);
    ImGui::PopStyleColor();
  }

}; // end of struct BooleanLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BOOLEAN_LINE_COMPONENT_H
