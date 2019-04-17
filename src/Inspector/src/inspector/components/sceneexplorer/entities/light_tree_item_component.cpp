#include <babylon/inspector/components/sceneexplorer/entities/light_tree_item_component.h>

#include <babylon/core/string.h>
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

  // Set the specialized component info
  specializedComponentInfo.uniqueId = light->uniqueId;
  const auto className              = light->getClassName();
  if (String::contains(className, "DirectionalLight")) {
    specializedComponentInfo.entityType = EntityType::DirectionalLight;
  }
  else if (String::contains(className, "HemisphericLight")) {
    specializedComponentInfo.entityType = EntityType::HemisphericLight;
  }
  else if (String::contains(className, "PointLight")) {
    specializedComponentInfo.entityType = EntityType::PointLight;
  }
  else if (String::contains(className, "SpotLight")) {
    specializedComponentInfo.entityType = EntityType::SpotLight;
  }
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

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::IconSizeDouble);
  ImGui::TextWrapped("%s", faLightbubRegular);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Turn on/off the light");
  }

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::IconSizeHalf);
  float isNotActive = 0.3f;
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, isNotActive));
  ImGui::TextWrapped("%s", faEye);
  ImGui::PopStyleColor();
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", "Turn on/off the light's gizmo");
  }
}

} // end of namespace BABYLON
