#include <babylon/inspector/components/sceneexplorer/entities/skeleton_tree_item_component.h>

#include <babylon/bones/skeleton.h>
#include <babylon/core/string.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

SkeletonTreeItemComponent::SkeletonTreeItemComponent(
  const ISkeletonTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& skeleton = props.skeleton;

  sprintf(label, "%s",
          !skeleton->name.empty() ? skeleton->name.c_str() : "no name");

  // Set the entity info
  entityInfo.uniqueId = skeleton->uniqueId;
  entityInfo.name     = skeleton->name;
  entityInfo.type     = EntityType::Skeleton;
}

SkeletonTreeItemComponent::~SkeletonTreeItemComponent() = default;

void SkeletonTreeItemComponent::render()
{
  // Texture tree item label
  TreeItemLabelComponent::render(label, faSkull, ImGui::gray);
}

} // end of namespace BABYLON
