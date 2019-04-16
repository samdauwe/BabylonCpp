#include <babylon/inspector/components/sceneexplorer/entities/scene_tree_item_component.h>

#include <cstdio>

namespace BABYLON {

SceneTreeItemComponent::SceneTreeItemComponent(
  const ISceneTreeItemComponentProps& iProps)
    : props{iProps}
{
  sprintf(label, "%s %s", faImage, "Scene");
}

SceneTreeItemComponent::~SceneTreeItemComponent()
{
}

void SceneTreeItemComponent::componentWillMount()
{
}

void SceneTreeItemComponent::componentWillUnmount()
{
}

void SceneTreeItemComponent::onSelect()
{
}

void SceneTreeItemComponent::onPickingMode()
{
}

void SceneTreeItemComponent::setGizmoMode(unsigned int /*mode*/)
{
}

void SceneTreeItemComponent::renderLabelWithIcon()
{
}

void SceneTreeItemComponent::renderControls()
{
}

} // end of namespace BABYLON
