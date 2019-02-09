#include <babylon/inspector/components/sceneexplorer/entities/scene_tree_item_component.h>

namespace BABYLON {

SceneTreeItemComponent::SceneTreeItemComponent(
  const ISceneTreeItemComponentProps& iProps)
    : props{iProps}
{
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

void SceneTreeItemComponent::render()
{
}

} // end of namespace BABYLON
