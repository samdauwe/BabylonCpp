#include <babylon/core/logging/init_console_logger.h>
#include <babylon/core/logging.h>
#include <memory>

#if _MSC_VER
#include <windows.h>
#endif
namespace BABYLON
{

namespace impl
{
  struct ConsoleLogger
  {
    using log_message_t = SA::delegate<void(const BABYLON::LogMessage&)>;

    static void onLogMessage(const BABYLON::LogMessage& logMessage)
    {
      printf("%s\n", logMessage.toString().c_str());
#if _MSC_VER
      std::string msg_n = logMessage.toString() + "\n";
      OutputDebugString(msg_n.c_str());
#endif
    }

  }; // end of struct ConsoleLogger

  // global logger
  std::unique_ptr<ConsoleLogger::log_message_t> gLogListenerDelegate;

  void initConsoleLogger()
  {
    gLogListenerDelegate = std::make_unique<ConsoleLogger::log_message_t>(SA::delegate<void(
      const BABYLON::LogMessage&)>::create<&ConsoleLogger::onLogMessage>());

    static_assert(
      std::is_same< std::unique_ptr<SA::delegate<void(const BABYLON::LogMessage&)>>,
      decltype(gLogListenerDelegate)>::value,
      "!");
    // Intialize log levels
    std::vector<std::pair<unsigned int, std::string>> _logLevels;
    for (auto& logLevel : BABYLON::LogLevels::Levels) {
      unsigned int logType = logLevel.first;
      if ((logType != BABYLON::LogLevels::LEVEL_QUIET)
        && (logType != BABYLON::LogLevels::LEVEL_TRACE)) {
        _logLevels.emplace_back(logLevel);
      }
    }
    // Subscribe to channels
    for (auto& logLevel : _logLevels) {
      unsigned int logType = logLevel.first;
      if (logType != BABYLON::LogLevels::LEVEL_QUIET) {
        BABYLON::LoggerInstance().registerLogMessageListener(
          logType, *gLogListenerDelegate);
      }
    }
  }

} // namespace impl

void initConsoleLogger()
{
  impl::initConsoleLogger();
}

} // namespace BABYLON