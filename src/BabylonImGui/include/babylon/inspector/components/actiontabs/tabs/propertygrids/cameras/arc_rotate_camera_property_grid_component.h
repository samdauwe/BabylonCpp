#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_ARC_ROTATE_CAMERA_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_ARC_ROTATE_CAMERA_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/common_camera_property_grid_component.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ArcRotateCameraPropertyGridComponent {

  static void render(const ArcRotateCameraPtr& camera)
  {
    CommonCameraPropertyGridComponent::render(std::static_pointer_cast<Camera>(camera));
    // --- TRANSFORMS ---
    static auto transformsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORMS")) {
      auto sliderChange = SliderLineComponent::render(
        "Alpha", camera->alpha,
        camera->lowerAlphaLimit.has_value() ? *camera->lowerAlphaLimit : 0.f,
        camera->upperAlphaLimit.has_value() ? *camera->upperAlphaLimit : Math::PI2, 0.01f, "%.3f");
      if (sliderChange) {
        camera->alpha = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render("Beta", camera->beta, camera->lowerBetaLimit,
                                                 camera->upperBetaLimit, 0.01f, "%.3f");
      if (sliderChange) {
        camera->beta = sliderChange.value();
      }
      auto valueChange = FloatLineComponent::render("Radius", camera->radius);
      if (valueChange) {
        camera->radius = valueChange.value();
      }
      transformsContainerOpened = true;
    }
    else {
      transformsContainerOpened = false;
    }
    // --- CONTROLS ---
    static auto controlsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(controlsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("CONTROLS")) {
      auto valueChange = FloatLineComponent::render("Speed", camera->speed);
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
      Vector3LineComponent::render("Collision radius", *camera->collisionRadius);
      collisionsContainerOpened = true;
    }
    else {
      collisionsContainerOpened = false;
    }
    // --- LIMITS ---
    static auto limitsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(limitsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LIMITS")) {
      auto valueChange = FloatLineComponent::render("Lower alpha limit", *camera->lowerAlphaLimit);
      if (valueChange) {
        camera->lowerAlphaLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Upper alpha limit", *camera->upperAlphaLimit);
      if (valueChange) {
        camera->upperAlphaLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Lower beta limit", camera->lowerBetaLimit);
      if (valueChange) {
        camera->lowerBetaLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Upper beta limit", camera->upperBetaLimit);
      if (valueChange) {
        camera->upperBetaLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Lower radius limit", *camera->lowerRadiusLimit);
      if (valueChange) {
        camera->lowerRadiusLimit = valueChange.value();
      }
      valueChange = FloatLineComponent::render("Upper radius limit", *camera->upperRadiusLimit);
      if (valueChange) {
        camera->upperRadiusLimit = valueChange.value();
      }
      limitsContainerOpened = true;
    }
    else {
      limitsContainerOpened = false;
    }
    // --- BEHAVIORS ---
    static auto behaviorsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(behaviorsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("BEHAVIORS")) {
      if (CheckBoxLineComponent::render("Auto rotation", camera->useAutoRotationBehavior())) {
        camera->useAutoRotationBehavior = !camera->useAutoRotationBehavior();
      }
      if (CheckBoxLineComponent::render("Bouncing", camera->useBouncingBehavior())) {
        camera->useBouncingBehavior = !camera->useBouncingBehavior;
      }
      if (CheckBoxLineComponent::render("Framing", camera->useFramingBehavior())) {
        camera->useFramingBehavior = !camera->useFramingBehavior();
      }
      behaviorsContainerOpened = true;
    }
    else {
      behaviorsContainerOpened = false;
    }
  }

}; // end of struct ArcRotateCameraPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_CAMERAS_ARC_ROTATE_CAMERA_PROPERTY_GRID_COMPONENT_H
