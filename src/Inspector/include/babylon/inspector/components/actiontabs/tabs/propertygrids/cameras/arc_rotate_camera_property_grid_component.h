#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_ARC_ROTATE_CAMERA_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_ARC_ROTATE_CAMERA_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/common_camera_property_grid_component.h>

namespace BABYLON {

class ArcRotateCamera;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;

struct BABYLON_SHARED_EXPORT ArcRotateCameraPropertyGridComponent {

  static void render(const ArcRotateCameraPtr& camera)
  {
    CommonCameraPropertyGridComponent::render(
      std::static_pointer_cast<Camera>(camera));
    // --- TRANSFORMS ---
    static auto transformsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORMS")) {
      SliderLineComponent::render(
        "Alpha", camera->alpha,
        camera->lowerAlphaLimit.has_value() ? *camera->lowerAlphaLimit : 0.f,
        camera->upperAlphaLimit.has_value() ? *camera->upperAlphaLimit :
                                              Math::PI2,
        0.01f, "%.3f");
      SliderLineComponent::render("Beta", camera->beta, camera->lowerBetaLimit,
                                  camera->upperBetaLimit, 0.01f, "%.3f");
      FloatLineComponent::render("Radius", camera->radius);
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
      Vector3LineComponent::render("Ellipsoid radius",
                                   *camera->collisionRadius);
      collisionsContainerOpened = true;
    }
    else {
      collisionsContainerOpened = false;
    }
    // --- LIMITS ---
    static auto limitsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(limitsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LIMITS")) {
      FloatLineComponent::render("Lower alpha limit", *camera->lowerAlphaLimit);
      FloatLineComponent::render("Upper alpha limit", *camera->upperAlphaLimit);
      FloatLineComponent::render("Lower beta limit", camera->lowerBetaLimit);
      FloatLineComponent::render("Upper beta limit", camera->upperBetaLimit);
      FloatLineComponent::render("Lower radius limit",
                                 *camera->lowerRadiusLimit);
      FloatLineComponent::render("Upper radius limit",
                                 *camera->upperRadiusLimit);
      limitsContainerOpened = true;
    }
    else {
      limitsContainerOpened = false;
    }
    // --- BEHAVIORS ---
    static auto behaviorsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(behaviorsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("BEHAVIORS")) {
      CheckBoxLineComponent::render(
        "Auto rotation", camera->useAutoRotationBehavior(),
        [&camera](bool value) { camera->useAutoRotationBehavior = value; });
      CheckBoxLineComponent::render(
        "Bouncing", camera->useBouncingBehavior(),
        [&camera](bool value) { camera->useBouncingBehavior = value; });
      CheckBoxLineComponent::render(
        "Framing", camera->useFramingBehavior(),
        [&camera](bool value) { camera->useFramingBehavior = value; });
      behaviorsContainerOpened = true;
    }
    else {
      behaviorsContainerOpened = false;
    }
  }

}; // end of struct ArcRotateCameraPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_ARC_ROTATE_CAMERA_PROPERTY_GRID_COMPONENT_H
