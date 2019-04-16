#include <babylon/inspector/components/sceneexplorer/entities/mesh_tree_item_component.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

MeshTreeItemComponent::MeshTreeItemComponent(
  const IMeshTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& mesh = props.mesh;

  labelWithoutIcon     = props.mesh->name;
  state.isVisible      = mesh->isVisible;
  state.isGizmoEnabled = false;
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

void MeshTreeItemComponent::renderLabelWithIcon()
{
  static ImVec4 dodgerblue = ImColor(0.0f, 0.0f, 1.0f, 1.0f);
  TreeItemLabelComponent::render(labelWithoutIcon.c_str(), faCube, dodgerblue);
}

void MeshTreeItemComponent::renderControls()
{
  if (ImGui::Button(faVectorSquare)) {
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Show/Hide position gizmo");
  }
  ImGui::SameLine();
  if (ImGui::Button(state.isVisible ? faEye : faEyeSlash)) {
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Show/Hide mesh");
  }
}

} // end of namespace BABYLON
