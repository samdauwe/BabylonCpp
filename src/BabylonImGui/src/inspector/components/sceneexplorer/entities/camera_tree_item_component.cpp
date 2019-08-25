#include <babylon/inspector/components/sceneexplorer/entities/camera_tree_item_component.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <imgui_utils/imgui_utils.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_icon_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>

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

CameraTreeItemComponent::~CameraTreeItemComponent()
{
}

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
}

void CameraTreeItemComponent::componentWillUnmount()
{
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
