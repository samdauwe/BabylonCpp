#include <babylon/inspector/components/sceneexplorer/entities/camera_tree_item_component.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/imgui/imgui_utils.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>

namespace BABYLON {

CameraTreeItemComponent::CameraTreeItemComponent(
  const ICameraTreeItemComponentProps& iProps)
    : props{iProps}, _onActiveCameraObserver{nullptr}
{
  const auto& camera = props.camera;
  const auto& scene  = camera->getScene();

  sprintf(label, "%s", props.camera->name.c_str());
  state.isActive = scene->activeCamera == camera;
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
  static ImVec4 green = ImColor(0.0f, 1.0f, 0.0f, 1.0f);
  TreeItemLabelComponent::render(label, faCamera, green);

  ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::DoubleIconSize);

  ImGui::TextWrapped("%s", faVideo);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Set as main camera");
  }
}

} // end of namespace BABYLON
