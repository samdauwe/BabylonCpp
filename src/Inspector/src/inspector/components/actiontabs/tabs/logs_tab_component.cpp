#include <babylon/inspector/components/actiontabs/tabs/logs_tab_component.h>

#include <babylon/core/logging.h>
#include <babylon/imgui/imgui_utils.h>

#include <iostream>

namespace BABYLON {

// Data
static std::vector<LogMessage> _messages;
static auto logListenerDelegate = SA::delegate<void(const LogMessage&)>{
  [](const BABYLON::LogMessage& logMessage) {
    _messages.emplace_back(logMessage);
  }};

LogsTabComponent::LogsTabComponent(const IPaneComponentProps& iProps)
    : PaneComponent{iProps}
{
  componentWillMount();
}

LogsTabComponent::~LogsTabComponent()
{
  componentWillUnmount();
}

void LogsTabComponent::componentWillMount()
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

void LogsTabComponent::componentWillUnmount()
{
  Logger::Instance().unregisterLogMessageListener(logListenerDelegate);
}

void LogsTabComponent::render()
{
  // Clear log messages
  if (ImGui::Button("Clear")) {
    // _messages.clear();
    BABYLON_LOG_INFO("LogsTabComponent", "Clicked");
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
      ImGui::TextWithColors("{%s}[%s]: %s",
                            (message.level() == LogLevels::LEVEL_INFO) ?
                              LEVEL_INFO_COLOR :
                              (message.level() == LogLevels::LEVEL_WARN) ?
                              LEVEL_WARN_COLOR :
                              (message.level() == LogLevels::LEVEL_ERROR) ?
                              LEVEL_ERROR_COLOR :
                              (message.level() == LogLevels::LEVEL_DEBUG) ?
                              LEVEL_DEBUG_COLOR :
                              (message.level() == LogLevels::LEVEL_TRACE) ?
                              LEVEL_TRACE_COLOR :
                              LEVEL_OTHER_COLOR,
                            message.getReadableTimestamp().substr(11).c_str(),
                            message.message().c_str());
    }
    ImGui::SetScrollHere(1.0f);
  }
  ImGui::EndChild();
}

} // end of namespace BABYLON
