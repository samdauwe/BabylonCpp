#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_QUATERNION_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_QUATERNION_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/maths/quaternion.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT QuaternionLineComponent {

  static constexpr float CharacterWidth = 7.f;

  static void render(const char* label, Quaternion& quaternion)
  {
    ImGui::TextWrapped("%s", label);
    char valueStr[64] = {};
    sprintf(
      valueStr, "X: %0.2f, Y: %0.2f, Z: %0.2f, W: %0.2f",
      static_cast<double>(quaternion.x), static_cast<double>(quaternion.y),
      static_cast<double>(quaternion.z), static_cast<double>(quaternion.w));
    const auto strLen = strlen(valueStr) + 1;
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - strLen * QuaternionLineComponent::CharacterWidth);
    ImGui::TextWrapped("%s", valueStr);
  }

}; // end of struct QuaternionLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_QUATERNION_LINE_COMPONENT_H
