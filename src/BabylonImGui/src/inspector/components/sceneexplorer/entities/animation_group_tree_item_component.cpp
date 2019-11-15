#include <babylon/inspector/components/sceneexplorer/entities/animation_group_tree_item_component.h>

#include <babylon/animations/animation_group.h>
#include <babylon/core/string.h>
#include <imgui_utils/imgui_utils.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>

namespace BABYLON {

AnimationGroupTreeItemComponent::AnimationGroupTreeItemComponent(
  const IAnimationGroupTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& animationGroup = props.animationGroup;

  sprintf(label, "%s", animationGroup->name.c_str());

  // Set the entity info
  entityInfo.name = animationGroup->name;
  entityInfo.type = EntityType::Bone;
}

AnimationGroupTreeItemComponent::~AnimationGroupTreeItemComponent() = default;

void AnimationGroupTreeItemComponent::render()
{
  // Texture tree item label
  TreeItemLabelComponent::render(label, faFilm, ImGui::cornflowerblue);
}

} // end of namespace BABYLON
