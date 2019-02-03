#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H

#include <babylon/babylon_api.h>

#include <babylon/inspector/components/actiontabs/pane_component.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PropertyGridTabComponent : public PaneComponent {

public:
  PropertyGridTabComponent(const IPaneComponentProps& props);
  ~PropertyGridTabComponent() override;

  void componentWillMount();
  void componentWillUnmount();
  void render() override;

}; // end of class StatisticsTabComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H
