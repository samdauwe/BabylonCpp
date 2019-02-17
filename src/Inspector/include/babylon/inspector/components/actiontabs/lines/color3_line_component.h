#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_COLOR3_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_COLOR3_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT Color3LineComponent {

  static void render(const char* label, Color3& color)
  {
    float col3[3] = {color.r, color.g, color.b};
    if (ImGui::ColorEdit3(label, col3)) {
      color.set(col3[0], col3[1], col3[2]);
    }
  }

  static void render(const char* label, Color4& color)
  {
    float col3[3] = {color.r, color.g, color.b};
    if (ImGui::ColorEdit3(label, col3)) {
      color.set(col3[0], col3[1], col3[2], color.a);
    }
  }

}; // end of struct Color3LineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_COLOR3_LINE_COMPONENT_H
