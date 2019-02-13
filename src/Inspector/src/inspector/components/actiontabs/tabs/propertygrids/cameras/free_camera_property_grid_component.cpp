#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/free_camera_property_grid_component.h>

#include <imgui.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/common_camera_property_grid_component.h>

namespace BABYLON {

FreeCameraPropertyGridComponent::FreeCameraPropertyGridComponent(
  const IFreeCameraPropertyGridComponentProps& iProps)
    : props{iProps}
{
  ICommonCameraPropertyGridComponentProps commonCameraProps;
  commonCameraProps.camera = std::static_pointer_cast<Camera>(props.camera);
  _commonCameraPropertyGridComponent
    = std::make_unique<CommonCameraPropertyGridComponent>(commonCameraProps);
}

FreeCameraPropertyGridComponent::~FreeCameraPropertyGridComponent()
{
}

void FreeCameraPropertyGridComponent::render()
{
  // const auto& camera = props.camera;

  if (_commonCameraPropertyGridComponent) {
    _commonCameraPropertyGridComponent->render();
  }
  // --- TRANSFORMS ---
  static auto transformsOpened = true;
  ImGui::SetNextTreeNodeOpen(transformsOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("TRANSFORMS")) {
    transformsOpened = true;
  }
  else {
    transformsOpened = false;
  }
  // --- CONTROLS ---
  static auto controlsOpened = true;
  ImGui::SetNextTreeNodeOpen(controlsOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("CONTROLS")) {
    controlsOpened = true;
  }
  else {
    controlsOpened = false;
  }
  // --- COLLISIONS ---
  static auto collisionsOpened = true;
  ImGui::SetNextTreeNodeOpen(collisionsOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("COLLISIONS")) {
    collisionsOpened = true;
  }
  else {
    collisionsOpened = false;
  }
}

} // end of namespace BABYLON
