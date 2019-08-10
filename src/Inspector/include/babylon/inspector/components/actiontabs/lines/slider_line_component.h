#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_SLIDER_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_SLIDER_LINE_COMPONENT_H

#include <optional>

#include <babylon/babylon_api.h>
#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SliderLineComponent {

  static constexpr float Width = 241.f;

  static std::optional<float> render(const char* label, float value,
                                     float minimum, float maximum, float step,
                                     const char* displayFormat = nullptr)
  {
    std::optional<float> result = std::nullopt;

    ImGui::TextWrapped("%s", label);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                    - SliderLineComponent::Width);
    ImGui::PushID(label);
    ImGui::PushItemWidth(SliderLineComponent::Width);
    if (ImGui::SliderFloatWithSteps("", &value, minimum, maximum, step,
                                    displayFormat)) {
      result = value;
    }
    ImGui::PopID();

    return result;
  }

}; // end of struct SliderLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_SLIDER_LINE_COMPONENT_H
