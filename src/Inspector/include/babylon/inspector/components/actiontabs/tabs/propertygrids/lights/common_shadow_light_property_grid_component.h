#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_SHADOW_LIGHT_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_SHADOW_LIGHT_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/lights/ishadow_light.h>

namespace BABYLON {

class IShadowLight;
using IShadowLightPtr = std::shared_ptr<IShadowLight>;

struct BABYLON_SHARED_EXPORT CommonShadowLightPropertyGridComponent {

  static void render(const IShadowLightPtr& light)
  {
    // --- SHADOWS ---
    static auto shadowsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(shadowsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("SHADOWS")) {
      CheckBoxLineComponent::render(
        "Shadows enabled", light->shadowEnabled(),
        [&light](bool value) { light->shadowEnabled = value; });
      shadowsContainerOpened = true;
    }
    else {
      shadowsContainerOpened = false;
    }
  }

}; // end of struct CommonShadowLightPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_SHADOW_LIGHT_PROPERTY_GRID_COMPONENT_H
