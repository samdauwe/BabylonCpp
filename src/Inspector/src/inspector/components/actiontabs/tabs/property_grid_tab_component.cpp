#include <babylon/inspector/components/actiontabs/tabs/property_grid_tab_component.h>

#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/scene_property_grid_component.h>

namespace BABYLON {

PropertyGridTabComponent::PropertyGridTabComponent(
  const IPaneComponentProps& iProps)
    : PaneComponent{iProps}, _scenePropertyGridComponent{nullptr}
{
  componentWillMount();
}

PropertyGridTabComponent::~PropertyGridTabComponent()
{
  componentWillUnmount();
}

void PropertyGridTabComponent::componentWillMount()
{
  IScenePropertyGridComponentProps scenePropertyGridComponentProps;
  scenePropertyGridComponentProps.scene = props.scene;
  _scenePropertyGridComponent = std::make_unique<ScenePropertyGridComponent>(
    scenePropertyGridComponentProps);
}

void PropertyGridTabComponent::componentWillUnmount()
{
}

void PropertyGridTabComponent::render()
{
  if (_scenePropertyGridComponent) {
    _scenePropertyGridComponent->render();
  }
}

} // end of namespace BABYLON
