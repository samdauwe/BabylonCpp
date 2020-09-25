#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/common_camera_property_grid_component.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT FreeCameraPropertyGridComponent {

  static void render(const FreeCameraPtr& camera)
  {
    CommonCameraPropertyGridComponent::render(std::static_pointer_cast<Camera>(camera));
    // --- TRANSFORMS ---
    static auto transformsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORMS")) {
      Vector3LineComponent::render("Target", camera->target());
      Vector3LineComponent::render("Position", camera->position());
      Vector3LineComponent::render("Rotation", camera->rotation());
      transformsContainerOpened = true;
    }
    else {
      transformsContainerOpened = false;
    }
    // --- CONTROLS ---
    static auto controlsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(controlsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("CONTROLS")) {
      auto valueChange
        = FloatLineComponent::render("Angular sensitivity", camera->angularSensibility());
      if (valueChange) {
        camera->angularSensibility = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Speed", camera->speed);
      if (valueChange) {
        camera->speed = valueChange.value();
      }
      controlsContainerOpened = true;
    }
    else {
      controlsContainerOpened = false;
    }
    // --- COLLISIONS ---
    static auto collisionsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(collisionsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("COLLISIONS")) {
      if (CheckBoxLineComponent::render("Check collisions", camera->checkCollisions)) {
        camera->checkCollisions = !camera->checkCollisions;
      }
      if (CheckBoxLineComponent::render("Apply gravity", camera->applyGravity)) {
        camera->applyGravity = !camera->applyGravity;
      }
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
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H
