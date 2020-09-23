#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_LIGHT_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_LIGHT_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/button_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class Light;
using LightPtr = std::shared_ptr<Light>;

struct BABYLON_SHARED_EXPORT CommonLightPropertyGridComponent {

  static void render(const LightPtr& light)
  {
    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("ID", light->id);
      TextLineComponent::render("Unique ID", std::to_string(light->uniqueId));
      TextLineComponent::render("Class", light->getClassName());
      auto valueChange = FloatLineComponent::render("Intensity", light->intensity);
      if (valueChange) {
        light->intensity = valueChange.value();
      }
      if (ButtonLineComponent::render("Dispose")) {
        light->dispose();
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
  }

}; // end of struct CommonLightPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_LIGHT_PROPERTY_GRID_COMPONENT_H
