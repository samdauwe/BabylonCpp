#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_FLOAT_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_FLOAT_LINE_COMPONENT_H

#include <optional>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT FloatLineComponent {

  static constexpr float Width = 241.f;

  static std::optional<float> render(const char* label, float value)
  {
    std::optional<float> result = std::nullopt;

    ImGui::TextWrapped("%s", label);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - FloatLineComponent::Width);
    ImGui::PushID(label);
    ImGui::PushItemWidth(FloatLineComponent::Width);
    if (ImGui::InputFloat(label, &value, 0.01f, 0.1f, "%.3f")) {
      result = value;
    }
    ImGui::PopID();

    return result;
  }

  static void render(const char* label, float value,
                     const SA::delegate<void(float value)>& onChange)
  {
    if (ImGui::InputFloat(label, &value, 0.01f, 0.1f, "%.3f")) {
      onChange(value);
    }
  }

}; // end of struct FloatLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_FLOAT_LINE_COMPONENT_H
