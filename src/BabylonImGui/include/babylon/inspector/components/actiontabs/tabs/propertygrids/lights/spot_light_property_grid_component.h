#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_SPOT_LIGHT_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_SPOT_LIGHT_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/common_light_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/common_shadow_light_property_grid_component.h>
#include <babylon/lights/spot_light.h>

namespace BABYLON {

class SpotLight;
using SpotLightPtr = std::shared_ptr<SpotLight>;

struct BABYLON_SHARED_EXPORT SpotLightPropertyGridComponent {

  static void render(const SpotLightPtr& light)
  {
    CommonLightPropertyGridComponent::render(std::static_pointer_cast<Light>(light));
    // --- SETUP ---
    static auto setupContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(setupContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("SETUP")) {
      Color3LineComponent::render("Diffuse", light->diffuse);
      Color3LineComponent::render("Specular", light->specular);
      Vector3LineComponent::render("Position", light->position);
      Vector3LineComponent::render("Direction", light->direction);
      auto valueChange = FloatLineComponent::render("Angle", light->angle());
      if (valueChange) {
        light->angle = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Inner angle", light->innerAngle());
      if (valueChange) {
        light->innerAngle = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Exponent", light->exponent);
      if (valueChange) {
        light->exponent = valueChange.value();
      }
      setupContainerOpened = true;
    }
    else {
      setupContainerOpened = false;
    }
    CommonShadowLightPropertyGridComponent::render(light);
  }

}; // end of struct SpotLightPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_SPOT_LIGHT_PROPERTY_GRID_COMPONENT_H
