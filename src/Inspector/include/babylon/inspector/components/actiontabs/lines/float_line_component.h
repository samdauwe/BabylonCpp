#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_FLOAT_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_FLOAT_LINE_COMPONENT_H

#include <string>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT FloatLineComponent {

  static void render(const std::string& label, float& value)
  {
    ImGui::InputFloat(label.c_str(), &value, 0.01f, 0.1f, "%.3f");
  }

}; // end of struct FloatLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_FLOAT_LINE_COMPONENT_H
