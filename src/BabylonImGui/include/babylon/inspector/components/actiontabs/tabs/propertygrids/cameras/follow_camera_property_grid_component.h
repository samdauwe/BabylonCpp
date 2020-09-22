#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_FOLLOW_CAMERA_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_FOLLOW_CAMERA_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/common_camera_property_grid_component.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT FollowCameraPropertyGridComponent {

  static void render(const FollowCameraPtr& camera)
  {
    CommonCameraPropertyGridComponent::render(std::static_pointer_cast<Camera>(camera));
    // --- TRANSFORMS ---
    static auto transformsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORMS")) {
      auto valueChange = FloatLineComponent::render("Radius", camera->radius);
      if (valueChange) {
        camera->radius = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Rotation offset", camera->rotationOffset);
      if (valueChange) {
        camera->rotationOffset = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Height offset", camera->heightOffset);
      if (valueChange) {
        camera->heightOffset = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Camera acceleration", camera->cameraAcceleration);
      if (valueChange) {
        camera->cameraAcceleration = valueChange.value();
      }
      transformsContainerOpened = true;
    }
    else {
      transformsContainerOpened = false;
    }
    // --- LIMITS ---
    static auto limitsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(limitsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LIMITS")) {
      auto valueChange
        = FloatLineComponent::render("Lower radius limit", *camera->lowerRadiusLimit);
      if (valueChange) {
        camera->lowerRadiusLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Upper radius limit", *camera->upperRadiusLimit);
      if (valueChange) {
        camera->upperRadiusLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Lower rotation offset limit",
                                               *camera->lowerRotationOffsetLimit);
      if (valueChange) {
        camera->lowerRotationOffsetLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Upper rotation offset limit",
                                               *camera->upperRotationOffsetLimit);
      if (valueChange) {
        camera->upperRotationOffsetLimit = valueChange.value();
      }
      valueChange
        = FloatLineComponent::render("Lower height offset limit", *camera->lowerHeightOffsetLimit);
      if (valueChange) {
        camera->lowerHeightOffsetLimit = valueChange.value();
      }
      valueChange
        = FloatLineComponent::render("Upper height offset limit", *camera->upperHeightOffsetLimit);
      if (valueChange) {
        camera->upperHeightOffsetLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Max camera speed", camera->maxCameraSpeed);
      if (valueChange) {
        camera->maxCameraSpeed = valueChange.value();
      }
      limitsContainerOpened = true;
    }
    else {
      limitsContainerOpened = false;
    }
  }

}; // end of struct FollowCameraPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_FOLLOW_CAMERA_PROPERTY_GRID_COMPONENT_H
