#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_RADIO_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_RADIO_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT RadioButtonLineComponent {

  static void render(const char* label, bool isSelected,
                     const SA::delegate<void()> onSelect)
  {
    if (ImGui::RadioButton(label, isSelected)) {
      onSelect();
    }
  }

}; // end of struct RadioButtonLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_RADIO_LINE_COMPONENT_H
