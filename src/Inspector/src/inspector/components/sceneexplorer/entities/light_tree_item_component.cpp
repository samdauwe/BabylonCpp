#include <babylon/inspector/components/sceneexplorer/entities/light_tree_item_component.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/lights/light.h>

namespace BABYLON {

LightTreeItemComponent::LightTreeItemComponent(
  const ILightTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& light = props.light;

  labelWithoutIcon     = props.light->name;
  state.isEnabled      = light->isEnabled();
  state.isGizmoEnabled = false;
}

LightTreeItemComponent::~LightTreeItemComponent()
{
}

void LightTreeItemComponent::switchIsEnabled()
{
}

void LightTreeItemComponent::toggleGizmo()
{
}

void LightTreeItemComponent::renderLabelWithIcon()
{
  static ImVec4 yellow = ImColor(1.0f, 1.0f, 0.0f, 1.0f);
  TreeItemLabelComponent::render(labelWithoutIcon.c_str(), faLightbulb, yellow);
}

void LightTreeItemComponent::renderControls()
{
  if (ImGui::Button(faLightbubRegular)) {
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Turn on/off the light");
  }
  ImGui::SameLine();
  if (ImGui::Button(faEye)) {
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Turn on/off the light's gizmo");
  }
}

} // end of namespace BABYLON
