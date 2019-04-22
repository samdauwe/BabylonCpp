#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_COLOR3_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_COLOR3_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT Color3LineComponent {

  static constexpr float Width = 241.f;

  static void render(const char* label, Color3& color)
  {
    float col3[3] = {color.r, color.g, color.b};
    ImGui::TextWrapped("%s", label);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - Color3LineComponent::Width);
    ImGui::PushID(label);
    ImGui::PushItemWidth(Color3LineComponent::Width);
    if (ImGui::ColorEdit3("", col3)) {
      color.set(col3[0], col3[1], col3[2]);
    }
    ImGui::PopID();
  }

  static void render(const char* label, const Color3& color,
                     const SA::delegate<void(const Color3& color)>& onChange)
  {
    float col3[3] = {color.r, color.g, color.b};
    if (ImGui::ColorEdit3(label, col3)) {
      onChange(Color3(col3[0], col3[1], col3[2]));
    }
  }

  static void render(const char* label, Color4& color)
  {
    float col4[4] = {color.r, color.g, color.b, color.a};
    ImGui::TextWrapped("%s", label);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - Color3LineComponent::Width);
    ImGui::PushID(label);
    ImGui::PushItemWidth(Color3LineComponent::Width);
    if (ImGui::ColorEdit4("", col4)) {
      color.set(col4[0], col4[1], col4[2], col4[3]);
    }
    ImGui::PopID();
  }

  static void render(const char* label, const Color4& color,
                     const SA::delegate<void(const Color4& color)>& onChange)
  {
    float col4[4] = {color.r, color.g, color.b, color.a};
    if (ImGui::ColorEdit4(label, col4)) {
      onChange(Color4(col4[0], col4[1], col4[2], col4[3]));
    }
  }

}; // end of struct Color3LineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_COLOR3_LINE_COMPONENT_H
