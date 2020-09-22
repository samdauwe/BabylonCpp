#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_COMMON_CAMERA_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_COMMON_CAMERA_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera.h>
#include <babylon/inspector/components/actiontabs/lines/button_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/options_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>

namespace BABYLON {

class Camera;
using CameraPtr = std::shared_ptr<Camera>;

struct BABYLON_SHARED_EXPORT CommonCameraPropertyGridComponent {

  static void render(const CameraPtr& camera)
  {
    static std::vector<std::pair<const char*, unsigned int>> modeOptions{
      {"Perspective", Camera::PERSPECTIVE_CAMERA},
      {"Orthographic", Camera::ORTHOGRAPHIC_CAMERA},
    };

    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("ID", camera->id);
      TextLineComponent::render("Unique ID", std::to_string(camera->uniqueId));
      TextLineComponent::render("Class", camera->getClassName());
      auto valueChange = FloatLineComponent::render("Near plane", camera->minZ);
      if (valueChange) {
        camera->minZ = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Far plane", camera->maxZ);
      if (valueChange) {
        camera->maxZ = valueChange.value();
      }
      auto sliderChange
        = SliderLineComponent::render("Inertia", camera->inertia, 0.f, 1.f, 0.01f, "%.3f");
      if (sliderChange) {
        camera->inertia = sliderChange.value();
      }
      auto optionChange = OptionsLineComponent::render("Mode", camera->mode, modeOptions);
      if (optionChange) {
        camera->mode = optionChange.value();
      }
      if (camera->mode == Camera::PERSPECTIVE_CAMERA) {
        SliderLineComponent::render("Field of view", camera->fov, 0.1f, Math::PI, 0.1f, "%.2f");
      }
      if (camera->mode == Camera::ORTHOGRAPHIC_CAMERA) {
        valueChange = FloatLineComponent::render("Left", camera->orthoLeft);
        if (valueChange) {
          camera->orthoLeft = valueChange.value();
        }
      }
      if (camera->mode == Camera::ORTHOGRAPHIC_CAMERA) {
        valueChange = FloatLineComponent::render("Top", camera->orthoTop);
        if (valueChange) {
          camera->orthoTop = valueChange.value();
        }
      }
      if (camera->mode == Camera::ORTHOGRAPHIC_CAMERA) {
        valueChange = FloatLineComponent::render("Right", camera->orthoRight);
        if (valueChange) {
          camera->orthoRight = valueChange.value();
        }
      }
      if (camera->mode == Camera::ORTHOGRAPHIC_CAMERA) {
        valueChange = FloatLineComponent::render("Bottom", camera->orthoBottom);
        if (valueChange) {
          camera->orthoBottom = valueChange.value();
        }
      }
      if (ButtonLineComponent::render("Dispose")) {
        camera->dispose();
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
  }

}; // end of struct CommonCameraPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_COMMON_CAMERA_PROPERTY_GRID_COMPONENT_H
