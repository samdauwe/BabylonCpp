#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_ACTIONS_TAB_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_ACTIONS_TAB_COMPONENT_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome_5.h>

namespace BABYLON {

class DebugTabComponent;
class PropertyGridTabComponent;
class Scene;
class StatisticsTabComponent;

struct BABYLON_SHARED_EXPORT IActionTabsComponentProps {
  Scene* scene = nullptr;
}; // end of struct IActionTabsComponentProps

class BABYLON_SHARED_EXPORT ActionTabsComponent {

public:
  /** Font Awesome Icon Unicodes **/
  // fa-file-alt
  static constexpr const char* faFileAlt = ICON_FA_FILE_ALT;
  // fa-wrench
  static constexpr const char* faWrench = ICON_FA_WRENCH;
  // fa-bug
  static constexpr const char* faBug = ICON_FA_BUG;
  // fa-chart-bar
  static constexpr const char* faChartBar = ICON_FA_CHART_BAR;

public:
  ActionTabsComponent(const IActionTabsComponentProps& props);
  virtual ~ActionTabsComponent();

  void componentWillMount();
  void componentWillUnmount();
  void render();

protected:
  IActionTabsComponentProps props;

private:
  std::unique_ptr<PropertyGridTabComponent> _propertyGridTabComponent;
  std::unique_ptr<DebugTabComponent> _debugTabComponent;
  std::unique_ptr<StatisticsTabComponent> _statisticsTabComponent;

}; // end of class ActionTabsComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_ACTIONS_TAB_COMPONENT_H
