#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_FLOAT_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_FLOAT_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT FloatLineComponent {

  static void render(const char* label, float& value)
  {
    ImGui::InputFloat(label, &value, 0.01f, 0.1f, "%.3f");
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
