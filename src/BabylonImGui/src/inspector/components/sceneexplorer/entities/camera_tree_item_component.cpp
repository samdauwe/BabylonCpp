#include <babylon/inspector/components/sceneexplorer/entities/camera_tree_item_component.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_icon_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

CameraTreeItemComponent::CameraTreeItemComponent(
  const ICameraTreeItemComponentProps& iProps)
    : props{iProps}, _onActiveCameraObserver{nullptr}
{
  const auto& camera = props.camera;
  const auto& scene  = camera->getScene();

  sprintf(label, "%s", props.camera->name.c_str());
  state.isActive = scene->activeCamera() == camera;

  // Set the entity info
  entityInfo.uniqueId  = camera->uniqueId;
  const auto className = camera->getClassName();
  if (String::contains(className, "ArcRotateCamera")) {
    entityInfo.type = EntityType::ArcRotateCamera;
  }
  else if (String::contains(className, "FreeCamera")) {
    entityInfo.type = EntityType::FreeCamera;
  }
  else if (String::contains(className, "UniversalCamera")) {
    entityInfo.type = EntityType::UniversalCamera;
  }
}

CameraTreeItemComponent::~CameraTreeItemComponent() = default;

void CameraTreeItemComponent::setActive()
{
  const auto& camera = props.camera;
  const auto& scene  = camera->getScene();

  scene->activeCamera = camera;
  camera->attachControl(scene->getEngine()->getRenderingCanvas(), true);

  state.isActive = true;
}

void CameraTreeItemComponent::componentWillMount()
{
  const auto& camera      = props.camera;
  const auto& scene       = camera->getScene();
  _onActiveCameraObserver = scene->onActiveCameraChanged.add(
    [this, &camera, &scene](Scene*, EventState&) -> void {
      // This will deactivate the previous camera when the camera is changed.
      // Multiple camera's cycle frequently so only do this for single cameras
      if (state.isActive && scene->activeCameras.size() <= 1) {
        camera->detachControl(scene->getEngine()->getRenderingCanvas());
      }
      state.isActive = scene->activeCamera() == camera;
    });
}

void CameraTreeItemComponent::componentWillUnmount()
{
  if (_onActiveCameraObserver) {
    const auto& camera = props.camera;
    const auto& scene  = camera->getScene();

    scene->onActiveCameraChanged.remove(_onActiveCameraObserver);
  }
}

void CameraTreeItemComponent::render()
{
  static auto scene = props.camera->getScene();

  // Camera tree item label
  TreeItemLabelComponent::render(label, faCamera, ImGui::green);

  // Active camera icon
  if (scene->activeCameras.empty()) {
    if (TreeItemIconComponent::render(faVideo, "Set as main camera",
                                      ImGui::GetWindowContentRegionWidth()
                                        - ImGui::IconSize,
                                      state.isActive)) {
      setActive();
    }
  }
}

} // end of namespace BABYLON
