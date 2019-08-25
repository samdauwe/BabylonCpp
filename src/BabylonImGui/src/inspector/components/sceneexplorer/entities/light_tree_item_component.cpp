#include <babylon/inspector/components/sceneexplorer/entities/light_tree_item_component.h>

#include <babylon/core/string.h>
#include <imgui_utils/imgui_utils.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_icon_component.h>
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

  // Set the entity info
  entityInfo.uniqueId  = light->uniqueId;
  const auto className = light->getClassName();
  if (String::contains(className, "DirectionalLight")) {
    entityInfo.type = EntityType::DirectionalLight;
  }
  else if (String::contains(className, "HemisphericLight")) {
    entityInfo.type = EntityType::HemisphericLight;
  }
  else if (String::contains(className, "PointLight")) {
    entityInfo.type = EntityType::PointLight;
  }
  else if (String::contains(className, "SpotLight")) {
    entityInfo.type = EntityType::SpotLight;
  }
}

LightTreeItemComponent::~LightTreeItemComponent()
{
}

void LightTreeItemComponent::switchIsEnabled()
{
  const auto& light = props.light;

  light->setEnabled(!light->isEnabled());

  state.isEnabled = light->isEnabled();
}

void LightTreeItemComponent::toggleGizmo()
{
}

void LightTreeItemComponent::render()
{
  // Light tree item label
  TreeItemLabelComponent::render(label, faLightbulb, ImGui::yellow);

  // Visibility icon
  if (TreeItemIconComponent::render(faLightbubRegular, "Turn on/off the light",
                                    ImGui::GetWindowContentRegionWidth()
                                      - 3.f * ImGui::IconSize,
                                    state.isEnabled)) {
    switchIsEnabled();
  }

  // Enable gizmo icon
  if (TreeItemIconComponent::render(faEye, "Turn on/off the light's gizmo",
                                    ImGui::GetWindowContentRegionWidth()
                                      - ImGui::IconSize,
                                    state.isGizmoEnabled)) {
    toggleGizmo();
  }
}

} // end of namespace BABYLON
