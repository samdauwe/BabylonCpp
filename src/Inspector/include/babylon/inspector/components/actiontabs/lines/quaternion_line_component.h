#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_QUATERNION_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_QUATERNION_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/math/quaternion.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT QuaternionLineComponent {

  static void render(const char* /*label*/, Quaternion& /*quaternion*/)
  {
  }

}; // end of struct QuaternionLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_QUATERNION_LINE_COMPONENT_H
