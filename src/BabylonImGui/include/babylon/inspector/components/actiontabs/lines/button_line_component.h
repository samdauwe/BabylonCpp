#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BUTTON_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BUTTON_LINE_COMPONENT_H

#include <babylon/babylon_api.h>

#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ButtonLineComponent {

  static bool render(const char* label)
  {
    return ImGui::Button(label);
  }

}; // end of struct ButtonLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_BUTTON_LINE_COMPONENT_H
