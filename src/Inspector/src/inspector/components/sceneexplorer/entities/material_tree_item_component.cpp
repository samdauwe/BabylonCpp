#include <babylon/inspector/components/sceneexplorer/entities/material_tree_item_component.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/materials/material.h>

namespace BABYLON {

MaterialTreeItemComponent::MaterialTreeItemComponent(
  const IMaterialTreeItemComponentProps& iProps)
    : props{iProps}
{
  labelWithoutIcon = props.material->name;
}

MaterialTreeItemComponent::~MaterialTreeItemComponent()
{
}

void MaterialTreeItemComponent::renderLabelWithIcon()
{
  static ImVec4 orange = ImColor(1.0f, 165.f / 255.f, 0.0f, 1.0f);
  TreeItemLabelComponent::render(labelWithoutIcon.c_str(), faBrush, orange);
}

void MaterialTreeItemComponent::renderControls()
{
}

} // end of namespace BABYLON
