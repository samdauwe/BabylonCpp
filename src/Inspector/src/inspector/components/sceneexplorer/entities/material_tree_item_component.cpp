#include <babylon/inspector/components/sceneexplorer/entities/material_tree_item_component.h>

#include <babylon/core/string.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/materials/material.h>

namespace BABYLON {

MaterialTreeItemComponent::MaterialTreeItemComponent(
  const IMaterialTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& material = props.material;

  sprintf(label, "%s", material->name.c_str());

  // Set the entity info
  entityInfo.uniqueId  = material->uniqueId;
  const auto className = material->getClassName();
  if (String::contains(className, "BackgroundMaterial")) {
    entityInfo.type = EntityType::BackgroundMaterial;
  }
  else if (String::contains(className, "PBRMaterial")) {
    entityInfo.type = EntityType::PBRMaterial;
  }
  else if (String::contains(className, "PBRMetallicRoughnessMaterial")) {
    entityInfo.type = EntityType::PBRMetallicRoughnessMaterial;
  }
  else if (String::contains(className, "PBRSpecularGlossinessMaterial")) {
    entityInfo.type = EntityType::PBRSpecularGlossinessMaterial;
  }
  else if (String::contains(className, "StandardMaterial")) {
    entityInfo.type = EntityType::StandardMaterial;
  }
  else if (String::contains(className, "Material")) {
    entityInfo.type = EntityType::Material;
  }
}

MaterialTreeItemComponent::~MaterialTreeItemComponent()
{
}

void MaterialTreeItemComponent::render()
{
  static ImVec4 orange = ImColor(1.0f, 165.f / 255.f, 0.0f, 1.0f);
  TreeItemLabelComponent::render(label, faBrush, orange);
}

} // end of namespace BABYLON
