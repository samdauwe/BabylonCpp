#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_LOGS_TAB_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_LOGS_TAB_COMPONENT_H

#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/pane_component.h>

namespace BABYLON {

class EngineInstrumentation;
class SceneInstrumentation;

class BABYLON_SHARED_EXPORT LogsTabComponent : public PaneComponent {

private:
  static constexpr const char* LEVEL_ERROR_COLOR = "ff0000"; // Red
  static constexpr const char* LEVEL_WARN_COLOR  = "ffa500"; // Orange
  static constexpr const char* LEVEL_INFO_COLOR  = "008000"; // Green
  static constexpr const char* LEVEL_DEBUG_COLOR = "800080"; // Purple
  static constexpr const char* LEVEL_TRACE_COLOR = "4c4cff"; // Blue
  static constexpr const char* LEVEL_OTHER_COLOR = "ffffff"; // white

public:
  LogsTabComponent(const IPaneComponentProps& props);
  ~LogsTabComponent() override;

  void componentWillMount();
  void componentWillUnmount();
  void render() override;

private:
  std::vector<std::pair<unsigned int, std::string>> _logLevels;
  std::unordered_map<unsigned int, bool> _subscribedLogChannels;

}; // end of class StatisticsTabComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_LOGS_TAB_COMPONENT_H
