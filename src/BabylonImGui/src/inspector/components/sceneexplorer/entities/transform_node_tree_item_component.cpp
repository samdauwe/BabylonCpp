#include <babylon/inspector/components/sceneexplorer/entities/transform_node_tree_item_component.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/meshes/transform_node.h>
#include <babylon/misc/string_tools.h>
#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

TransformNodeTreeItemComponent::TransformNodeTreeItemComponent(
  const ITransformNodeTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& transformNode = props.transformNode;

  sprintf(label, "%s", transformNode->name.c_str());

  // Set the entity info
  entityInfo.uniqueId  = transformNode->uniqueId;
  const auto className = transformNode->getClassName();
  if (StringTools::contains(className, "TransformNode")) {
    entityInfo.type = EntityType::TransformNode;
  }
  else if (StringTools::contains(className, "Mesh")) {
    entityInfo.type = EntityType::Mesh;
  }
}

TransformNodeTreeItemComponent::~TransformNodeTreeItemComponent() = default;

void TransformNodeTreeItemComponent::render()
{
  // TransformNode tree item label
  TreeItemLabelComponent::render(label, faCodeBranch, ImGui::cornflowerblue);
}

} // end of namespace BABYLON
