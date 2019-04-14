#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINE_CONTAINER_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINE_CONTAINER_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT LineContainerComponent {

  static bool begin(const char* title, bool& closed, bool border = false)
  {
    ImGui::SetNextTreeNodeOpen(!closed, ImGuiCond_Always);
    if (ImGui::CollapsingHeader(title)) {
      ImGui::Columns(2, title, border);
      closed = false;
    }
    else {
      closed = true;
    }
    return !closed;
  }

  static void end()
  {
    ImGui::Columns(1);
  }

}; // end of class LineContainerComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINE_CONTAINER_COMPONENT_H
