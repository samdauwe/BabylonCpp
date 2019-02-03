#include <babylon/inspector/components/actiontabs/tabs/property_grid_tab_component.h>

#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>

namespace BABYLON {

PropertyGridTabComponent::PropertyGridTabComponent(
  const IPaneComponentProps& iProps)
    : PaneComponent{iProps}
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
  TextLineComponent::render("This is the Poperties tab!");
}

} // end of namespace BABYLON
