#include <babylon/inspector/components/sceneexplorer/entities/mesh_tree_item_component.h>

#include <babylon/core/string.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_icon_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/meshes/abstract_mesh.h>
#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

MeshTreeItemComponent::MeshTreeItemComponent(
  const IMeshTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& mesh = props.mesh;

  sprintf(label, "%s", mesh->name.c_str());
  state.isBoundingBoxEnabled = mesh->showBoundingBox();
  state.isVisible            = mesh->isVisible;

  // Set the entity info
  entityInfo.uniqueId  = mesh->uniqueId;
  const auto className = mesh->getClassName();
  if (String::contains(className, "Mesh")) {
    entityInfo.type = EntityType::Mesh;
  }
  else if (String::contains(className, "TransformNode")) {
    entityInfo.type = EntityType::TransformNode;
  }
}

MeshTreeItemComponent::~MeshTreeItemComponent() = default;

void MeshTreeItemComponent::showBoundingBox()
{
  const auto& mesh           = props.mesh;
  mesh->showBoundingBox      = !state.isBoundingBoxEnabled;
  state.isBoundingBoxEnabled = !state.isBoundingBoxEnabled;
}

void MeshTreeItemComponent::switchVisibility()
{
  const auto newState   = !state.isVisible;
  state.isVisible       = newState;
  props.mesh->isVisible = newState;
}

void MeshTreeItemComponent::render()
{
  // Mesh tree item label
  TreeItemLabelComponent::render(label, faCube, ImGui::dodgerblue);

  // Enable bounding box icon
  if (TreeItemIconComponent::render(faSquare, "Show/Hide bounding box",
                                    ImGui::GetWindowContentRegionWidth()
                                      - 3.f * ImGui::IconSize,
                                    state.isBoundingBoxEnabled)) {
    showBoundingBox();
  }

  // Visibility icon
  if (TreeItemIconComponent::render(
        state.isVisible ? faEye : faEyeSlash, "Show/Hide mesh",
        ImGui::GetWindowContentRegionWidth() - ImGui::IconSize,
        state.isVisible)) {
    switchVisibility();
  }
}

} // end of namespace BABYLON
