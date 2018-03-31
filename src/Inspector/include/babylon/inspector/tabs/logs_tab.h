#ifndef BABYLON_INSPECTOR_TABS_LOGS_TAB_H
#define BABYLON_INSPECTOR_TABS_LOGS_TAB_H

#include <babylon/babylon_global.h>
#include <babylon/inspector/tabs/tab.h>

namespace BABYLON {

/**
 * @brief Displays all Babylon logs.
 */
class BABYLON_SHARED_EXPORT LogsTab : public Tab {

public:
  LogsTab();
  ~LogsTab() override;

  void render() override;
  void dispose() override;

private:
  void initialize();

private:
  std::vector<std::pair<unsigned int, std::string>> _logLevels;
  std::unordered_map<unsigned int, bool> _subscribedLogChannels;

}; // end of class LogsTab

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TABS_LOGS_TAB_H
