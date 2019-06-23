#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VECTOR3_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VECTOR3_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT Vector3LineComponent {

  static constexpr float CharacterWidth = 7.f;

  static void render(const char* label, Vector3& vector)
  {
    ImGui::TextWrapped("%s", label);
    char valueStr[64] = {};
    sprintf(valueStr, "X: %0.2f, Y: %0.2f, Z: %0.2f",
            static_cast<double>(vector.x), static_cast<double>(vector.y),
            static_cast<double>(vector.z));
    const auto strLen = strlen(valueStr) + 1;
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - strLen * Vector3LineComponent::CharacterWidth);
    ImGui::TextWrapped("%s", valueStr);
  }

}; // end of struct Vector3LineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_VECTOR3_LINE_COMPONENT_H
