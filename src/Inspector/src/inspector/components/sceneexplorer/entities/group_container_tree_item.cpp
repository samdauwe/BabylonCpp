#include <babylon/inspector/components/sceneexplorer/entities/group_container_tree_item.h>

#include <cstdio>

namespace BABYLON {

GroupContainerTreeItemComponent::GroupContainerTreeItemComponent(
  const char* iLabel)
{
  sprintf(label, "%s", iLabel);
}

GroupContainerTreeItemComponent::~GroupContainerTreeItemComponent()
{
}

void GroupContainerTreeItemComponent::renderLabelWithIcon()
{
}

void GroupContainerTreeItemComponent::renderControls()
{
}

} // end of namespace BABYLON
