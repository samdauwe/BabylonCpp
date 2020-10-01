#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_POINT_LIGHT_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_POINT_LIGHT_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/common_light_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/common_shadow_light_property_grid_component.h>
#include <babylon/lights/point_light.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PointLightPropertyGridComponent {

  static void render(const PointLightPtr& light)
  {
    CommonLightPropertyGridComponent::render(std::static_pointer_cast<Light>(light));
    // --- SETUP ---
    static auto setupContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(setupContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("SETUP")) {
      Color3LineComponent::render("Diffuse", light->diffuse);
      Color3LineComponent::render("Specular", light->specular);
      Vector3LineComponent::render("Position", light->position);
      setupContainerOpened = true;
    }
    else {
      setupContainerOpened = false;
    }
    CommonShadowLightPropertyGridComponent::render(light);
  }

}; // end of struct PointLightPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_POINT_LIGHT_PROPERTY_GRID_COMPONENT_H
