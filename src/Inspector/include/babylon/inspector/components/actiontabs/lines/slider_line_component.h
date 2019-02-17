#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_SLIDER_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_SLIDER_LINE_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>
#include <babylon/imgui/imgui_utils.h>

#include <iostream>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SliderLineComponent {

  static void render(const char* label, float& value, float minimum,
                     float maximum, float step,
                     const char* displayFormat = nullptr)
  {
    ImGui::SliderFloatWithSteps(label, &value, minimum, maximum, step,
                                displayFormat);
  }

  static void render(const char* label, float value, float minimum,
                     float maximum, float step,
                     const SA::delegate<void(float)>& onChange,
                     const char* displayFormat = nullptr)
  {
    if (ImGui::SliderFloatWithSteps(label, &value, minimum, maximum, step,
                                    displayFormat)) {
      onChange(value);
    }
  }

}; // end of struct SliderLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_SLIDER_LINE_COMPONENT_H
