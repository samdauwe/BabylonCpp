#include <babylon/inspector/components/sceneexplorer/entities/scene_tree_item_component.h>

#include <babylon/imgui/imgui_utils.h>
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
  TreeItemLabelComponent::render(label, faImage);

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                  - 12.f * ImGui::IconSize);
  ImGui::TextWrapped("%s", faArrowsAlt);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Enable/Disable position mode");
  }

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth()
                  - 10.f * ImGui::IconSize);
  ImGui::TextWrapped("%s", faRedoAlt);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Enable/Disable rotation mode");
  }

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 8.f * ImGui::IconSize);
  ImGui::TextWrapped("%s", faCompress);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Enable/Disable scaling mode");
  }

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 6.f * ImGui::IconSize);
  ImGui::TextWrapped("%s", faVectorSquare);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Enable/Disable bounding box mode");
  }

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 4.f * ImGui::IconSize);
  ImGui::TextWrapped("%s", "| ");

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::IconSizeDouble);
  ImGui::TextWrapped("%s", faCrosshairs);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Turn picking mode on/off");
  }

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::IconSizeHalf);
  ImGui::TextWrapped("%s", faSyncAlt);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Refresh the explorer");
  }
}

} // end of namespace BABYLON
