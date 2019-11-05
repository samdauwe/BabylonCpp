#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_ACTIONS_TAB_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_ACTIONS_TAB_COMPONENT_H

#include <memory>
#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <babylon/inspector/components/actiontabs/pane_component.h>
#include <babylon/inspector/entity.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class DebugTabComponent;
class GlobalState;
class LogsTabComponent;
class PropertyGridTabComponent;
class Scene;
class StatisticsTabComponent;
class SamplesBrowser;
using GlobalStatePtr = std::shared_ptr<GlobalState>;

struct BABYLON_SHARED_EXPORT IActionTabsComponentProps {
  Scene* scene               = nullptr;
  GlobalStatePtr globalState = nullptr;
}; // end of struct IActionTabsComponentProps


class BABYLON_SHARED_EXPORT ActionTabsComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faFileAlt    = ICON_FA_FILE_ALT;
  static constexpr const char* faWrench     = ICON_FA_WRENCH;
  static constexpr const char* faBug        = ICON_FA_BUG;
  static constexpr const char* faChartBar   = ICON_FA_CHART_BAR;
  static constexpr const char* faInfoCircle = ICON_FA_INFO_CIRCLE;

public:
  ActionTabsComponent(const IActionTabsComponentProps& props);
  virtual ~ActionTabsComponent();

  void setScene(Scene* scene);
  void componentWillMount();
  void componentWillUnmount();
  void render();

protected:
  IActionTabsComponentProps props;

private:
  // Shared pane properties
  IPaneComponentProps _paneProps;
  // Tabs
  std::unique_ptr<PropertyGridTabComponent> _propertyGridTabComponent;
  std::unique_ptr<DebugTabComponent> _debugTabComponent;
  std::unique_ptr<StatisticsTabComponent> _statisticsTabComponent;
  // Observers
  Observer<EntityInfo>::Ptr _onSelectionChangeObserver;

}; // end of class ActionTabsComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_ACTIONS_TAB_COMPONENT_H
