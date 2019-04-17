#include <babylon/inspector/components/sceneexplorer/entities/light_tree_item_component.h>

#include <babylon/imgui/imgui_utils.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/lights/light.h>

namespace BABYLON {

LightTreeItemComponent::LightTreeItemComponent(
  const ILightTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& light = props.light;

  sprintf(label, "%s", props.light->name.c_str());
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

void LightTreeItemComponent::render()
{
  static ImVec4 yellow = ImColor(1.0f, 1.0f, 0.0f, 1.0f);
  TreeItemLabelComponent::render(label, faLightbulb, yellow);

  ImGui::SameLine(ImGui::GetWindowWidth() - 2.f * ImGui::DoubleIconSize);
  ImGui::TextWrapped("%s", faLightbubRegular);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Turn on/off the light");
  }

  ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::DoubleIconSize);
  ImGui::TextWrapped("%s", faEye);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Turn on/off the light's gizmo");
  }
}

} // end of namespace BABYLON
