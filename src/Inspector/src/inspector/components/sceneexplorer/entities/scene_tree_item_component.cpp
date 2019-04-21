#include <babylon/inspector/components/sceneexplorer/entities/scene_tree_item_component.h>

#include <babylon/imgui/imgui_utils.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_icon_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>

namespace BABYLON {

SceneTreeItemComponent::SceneTreeItemComponent(
  const ISceneTreeItemComponentProps& iProps)
    : props{iProps}
{
  sprintf(label, "%s", "Scene");

  // Set the entity info
  entityInfo.uniqueId = 0;
  entityInfo.type     = EntityType::Scene;
}

SceneTreeItemComponent::~SceneTreeItemComponent()
{
}

void SceneTreeItemComponent::componentWillMount()
{
}

void SceneTreeItemComponent::componentWillUnmount()
{
}

void SceneTreeItemComponent::onSelect()
{
}

void SceneTreeItemComponent::onPickingMode()
{
}

void SceneTreeItemComponent::setGizmoMode(unsigned int /*mode*/)
{
}

void SceneTreeItemComponent::render()
{
  // Scene tree item label
  TreeItemLabelComponent::render(label, faImage);

  // Translation icon
  if (TreeItemIconComponent::render(faArrowsAlt, "Enable/Disable position mode",
                                    ImGui::GetWindowContentRegionWidth()
                                      - 6.f * ImGui::IconSizeDouble,
                                    state.gizmoMode == 1)) {
    setGizmoMode(1);
  }

  // Rotation icon
  if (TreeItemIconComponent::render(faRedoAlt, "Enable/Disable rotation mode",
                                    ImGui::GetWindowContentRegionWidth()
                                      - 5.f * ImGui::IconSizeDouble,
                                    state.gizmoMode == 2)) {
    setGizmoMode(2);
  }

  // Scaling icon
  if (TreeItemIconComponent::render(faCompress, "Enable/Disable scaling mode",
                                    ImGui::GetWindowContentRegionWidth()
                                      - 4.f * ImGui::IconSizeDouble,
                                    state.gizmoMode == 3)) {
    setGizmoMode(3);
  }

  // Bounding icon
  if (TreeItemIconComponent::render(
        faVectorSquare, "Enable/Disable bounding box mode",
        ImGui::GetWindowContentRegionWidth() - 3.f * ImGui::IconSizeDouble,
        state.gizmoMode == 4)) {
    setGizmoMode(4);
  }

  // Separator icon
  ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                  - 2.f * ImGui::IconSizeDouble
                  - (ImGui::GetStyle().ItemSpacing.x / 2.f));
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.48f, 0.72f, 1.0f));
  ImGui::TextWrapped("%s", " | ");
  ImGui::PopStyleColor();

  // Picking mode icon
  if (TreeItemIconComponent::render(faCrosshairs, "Turn picking mode on/off",
                                    ImGui::GetWindowContentRegionWidth()
                                      - 3.f * ImGui::IconSize,
                                    state.isInPickingMode)) {
    onPickingMode();
  }

  // Refresh icon
  if (TreeItemIconComponent::render(faSyncAlt, "Refresh the explorer",
                                    ImGui::GetWindowContentRegionWidth()
                                      - ImGui::IconSize)) {
    if (props.onRefresh) {
      props.onRefresh();
    }
  }
}

} // end of namespace BABYLON
