#include <babylon/inspector/components/sceneexplorer/entities/scene_tree_item_component.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>

namespace BABYLON {

SceneTreeItemComponent::SceneTreeItemComponent(
  const ISceneTreeItemComponentProps& iProps)
    : props{iProps}
{
  sprintf(label, "%s", "Scene");
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
  TreeItemLabelComponent::render(label, faImage);
}

} // end of namespace BABYLON
