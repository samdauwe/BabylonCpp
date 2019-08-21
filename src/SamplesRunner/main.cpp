#include <babylon/inspector/babylon_inspector_app.h>
#include "SamplesRunner/HelloScene.h"
#include <babylon/core/logging.h>
#include "chdir_to_exe.h"

#if _MSC_VER
#include <windows.h>
#endif
struct ConsoleLogger {

  using log_message_t = SA::delegate<void(const BABYLON::LogMessage&)>;

  static void onLogMessage(const BABYLON::LogMessage& logMessage)
  {
    printf("%s\n", logMessage.toString().c_str());
#if _MSC_VER
    std::string msg_n = logMessage.toString() + "\n";
    OutputDebugString(msg_n.c_str());
#endif
  }

  static log_message_t logListenerDelegate;

}; // end of struct ConsoleLogger

ConsoleLogger::log_message_t ConsoleLogger::logListenerDelegate
= SA::delegate<void(
  const BABYLON::LogMessage&)>::create<&ConsoleLogger::onLogMessage>();

/**
 * @brief Initializes the logging.
 */
void initializeLogging()
{
  static_assert(
    std::is_same<SA::delegate<void(const BABYLON::LogMessage&)>,
    decltype(ConsoleLogger::logListenerDelegate)>::value,
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
        logType, ConsoleLogger::logListenerDelegate);
    }
  }
}


int main(int , char** argv)
{
  // Enable console logging
  initializeLogging();

  ChdirToExe(argv[0]);
  BABYLON::BabylonInspectorApp app;
  app.RunApp(SamplesRunner::MakeHelloScene());
  return 0;
}
