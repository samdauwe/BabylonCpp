#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/common_camera_property_grid_component.h>

namespace BABYLON {

class FreeCamera;
using FreeCameraPtr = std::shared_ptr<FreeCamera>;

struct BABYLON_SHARED_EXPORT FreeCameraPropertyGridComponent {

  static void render(const FreeCameraPtr& camera)
  {
    CommonCameraPropertyGridComponent::render(
      std::static_pointer_cast<Camera>(camera));
    // --- TRANSFORMS ---
    static auto transformsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORMS")) {
      Vector3LineComponent::render("Position", camera->position);
      transformsContainerOpened = true;
    }
    else {
      transformsContainerOpened = false;
    }
    // --- CONTROLS ---
    static auto controlsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(controlsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("CONTROLS")) {
      FloatLineComponent::render("Speed", camera->speed);
      controlsContainerOpened = true;
    }
    else {
      controlsContainerOpened = false;
    }
    // --- COLLISIONS ---
    static auto collisionsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(collisionsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("COLLISIONS")) {
      CheckBoxLineComponent::render("Check collisions",
                                    camera->checkCollisions);
      CheckBoxLineComponent::render("Apply gravity", camera->applyGravity);
      Vector3LineComponent::render("Ellipsoid", camera->ellipsoid);
      Vector3LineComponent::render("Ellipsoid offset", camera->ellipsoidOffset);
      collisionsContainerOpened = true;
    }
    else {
      collisionsContainerOpened = false;
    }
  }

}; // end of struct FreeCameraPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H
