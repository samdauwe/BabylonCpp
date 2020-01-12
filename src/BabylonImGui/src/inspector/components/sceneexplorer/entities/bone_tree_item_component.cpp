#include <babylon/inspector/components/sceneexplorer/entities/bone_tree_item_component.h>

#include <babylon/bones/bone.h>
#include <babylon/misc/string_tools.h>
#include <imgui_utils/imgui_utils.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>

namespace BABYLON {

BoneTreeItemComponent::BoneTreeItemComponent(
  const IBoneTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& bone = props.bone;

  sprintf(label, "%s", !bone->name.empty() ? bone->name.c_str() : "no name");

  // Set the entity info
  entityInfo.uniqueId = bone->uniqueId;
  entityInfo.type     = EntityType::Bone;
}

BoneTreeItemComponent::~BoneTreeItemComponent() = default;

void BoneTreeItemComponent::render()
{
  // Texture tree item label
  TreeItemLabelComponent::render(label, faBone, ImGui::lightgray);
}

} // end of namespace BABYLON
