#include <babylon/inspector/components/actiontabs/tabs/property_grid_tab_component.h>

#include <imgui.h>

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
}

void PropertyGridTabComponent::componentWillUnmount()
{
}

void PropertyGridTabComponent::render()
{
  if (_scenePropertyGridComponent) {
    _scenePropertyGridComponent->render();
  }

  ImGui::TextWrapped("%s", "Please select an entity in the scene explorer.");
}

} // end of namespace BABYLON
