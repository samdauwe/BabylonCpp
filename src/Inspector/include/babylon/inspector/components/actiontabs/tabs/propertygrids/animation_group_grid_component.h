#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_GROUP_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_GROUP_GRID_COMPONENT_H

#include <memory>

#include <babylon/animations/animation_group.h>
#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>

namespace BABYLON {

class AnimationGroup;
using AnimationGroupPtr = std::shared_ptr<AnimationGroup>;

struct BABYLON_SHARED_EXPORT AnimationGroupGridComponent {

  static void render(const AnimationGroupPtr& /*animationGroup*/)
  {
    // --- CONTROLS ---
    static auto controlsOpened = true;
    ImGui::SetNextTreeNodeOpen(controlsOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("CONTROLS")) {
      controlsOpened = true;
    }
    else {
      controlsOpened = false;
    }
  }

}; // end of struct AnimationGroupGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_GROUP_GRID_COMPONENT_H
