#include <babylon/inspector/tabs/logs_tab.h>

#include <babylon/babylon_version.h>
#include <babylon/core/logging.h>
#include <babylon/imgui/imgui_dock.h>

namespace BABYLON {

// Data
static std::vector<LogMessage> _messages;
static auto logListenerDelegate = SA::delegate<void(const LogMessage&)>{
  [](const BABYLON::LogMessage& logMessage) {
    _messages.emplace_back(logMessage);
  }};

LogsTab::LogsTab() : Tab{"Logs"}
{
  initialize();
}

LogsTab::~LogsTab()
{
}

void LogsTab::initialize()
{
  static_assert(std::is_same<SA::delegate<void(const LogMessage&)>,
                             decltype(logListenerDelegate)>::value,
                "!");
  // Intialize log levels
  for (auto& logLevel : LogLevels::Levels) {
    unsigned int logType = logLevel.first;
    if ((logType != LogLevels::LEVEL_QUIET)
        && (logType != LogLevels::LEVEL_TRACE)) {
      _logLevels.emplace_back(logLevel);
    }
  }
  // Subscribe to channels
  for (auto& logLevel : _logLevels) {
    bool isSubscribed    = false;
    unsigned int logType = logLevel.first;
    if (logType == LogLevels::LEVEL_INFO) {
      Logger::Instance().registerLogMessageListener(LogLevels::LEVEL_INFO,
                                                    logListenerDelegate);
      isSubscribed = true;
    }
    _subscribedLogChannels[logType] = isSubscribed;
  }
}

void LogsTab::render()
{
  if (ImGui::BeginDock("Logs")) {
    // Clear logg messages
    if (ImGui::Button("Clear")) {
      _messages.clear();
    }
    ImGui::SameLine();
    // Channel filters
    for (auto& logLevel : _logLevels) {
      ImGui::SameLine();
      unsigned int channelType   = logLevel.first;
      std::string logLevelLabels = logLevel.second;
      bool subscribeToLogChannel = _subscribedLogChannels[channelType];
      if (ImGui::Checkbox(logLevelLabels.c_str(), &subscribeToLogChannel)) {
      }
      // Register / unregister
      if (subscribeToLogChannel != _subscribedLogChannels[channelType]) {
        if (subscribeToLogChannel) {
          Logger::Instance().registerLogMessageListener(channelType,
                                                        logListenerDelegate);
        }
        else {
          Logger::Instance().unregisterLogMessageListener(channelType,
                                                          logListenerDelegate);
        }
        _subscribedLogChannels[channelType]
          = Logger::Instance().isSubscribed(channelType, logListenerDelegate);
      }
    }
    // Log messages
    ImGui::Separator();
    if (ImGui::BeginChild("log_messages")) {
      for (auto& message : _messages) {
        // ImGui::TextWrapped("%s", message.toString().c_str());
        ImGui::TextWrapped("[%s]: %s",
                           message.getReadableTimestamp().substr(11).c_str(),
                           message.message().c_str());
      }
      ImGui::SetScrollHere(1.0f);
    }
    ImGui::EndChild();
  }
  ImGui::EndDock();
}

void LogsTab::dispose()
{
  Logger::Instance().unregisterLogMessageListener(logListenerDelegate);
}

} // end of namespace BABYLON
