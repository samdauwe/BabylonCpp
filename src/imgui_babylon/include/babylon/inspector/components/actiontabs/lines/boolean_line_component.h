#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BOOLEAN_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BOOLEAN_LINE_COMPONENT_H

#include <babylon/babylon_api.h>

#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BooleanLineComponent {

  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faCheck       = ICON_FA_CHECK;
  static constexpr const char* faTimesCircle = ICON_FA_TIMES_CIRCLE;

  static void render(const char* label, bool value)
  {
    ImGui::TextWrapped("%s", label);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - ImGui::IconSizeDouble);
    ImGui::PushStyleColor(ImGuiCol_Text, value ? ImGui::green : ImGui::red);
    ImGui::TextWrapped("%s", value ? faCheck : faTimesCircle);
    ImGui::PopStyleColor();
  }

}; // end of struct BooleanLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BOOLEAN_LINE_COMPONENT_H
