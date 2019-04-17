#include <babylon/inspector/components/sceneexplorer/entities/mesh_tree_item_component.h>

#include <babylon/core/string.h>
#include <babylon/imgui/imgui_utils.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

MeshTreeItemComponent::MeshTreeItemComponent(
  const IMeshTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& mesh = props.mesh;

  sprintf(label, "%s", props.mesh->name.c_str());
  state.isVisible      = mesh->isVisible;
  state.isGizmoEnabled = false;

  // Set the specialized component info
  specializedComponentInfo.uniqueId = mesh->uniqueId;
  const auto className              = mesh->getClassName();
  if (String::contains(className, "Mesh")) {
    specializedComponentInfo.entityType = EntityType::Mesh;
  }
  else if (String::contains(className, "TransformNode")) {
    specializedComponentInfo.entityType = EntityType::TransformNode;
  }
}

MeshTreeItemComponent::~MeshTreeItemComponent()
{
}

void MeshTreeItemComponent::showGizmos()
{
}

void MeshTreeItemComponent::switchVisibility()
{
}

void MeshTreeItemComponent::render()
{
  static ImVec4 dodgerblue = ImColor(0.0f, 0.0f, 1.0f, 1.0f);
  TreeItemLabelComponent::render(props.mesh->name.c_str(), faCube, dodgerblue);

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::IconSizeDouble);
  ImGui::TextWrapped("%s", faVectorSquare);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Show/Hide position gizmo");
  }

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::IconSizeHalf);
  ImGui::TextWrapped("%s", state.isVisible ? faEye : faEyeSlash);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Show/Hide mesh");
  }
}

} // end of namespace BABYLON
