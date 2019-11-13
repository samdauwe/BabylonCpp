#include <babylon/core/logging/logger.h>

#include <babylon/core/logging/log_message.h>

namespace BABYLON {

LogMessageHandler::LogMessageHandler()
    : _minLevel{LogLevels::LEVEL_QUIET}, _maxLevel{LogLevels::LEVEL_TRACE}
{
  for (unsigned int lvl = _minLevel; lvl <= _maxLevel; ++lvl) {
    _logMessageListeners[lvl] = std::vector<LogMessageListener*>();
  }
}

bool LogMessageHandler::takes(unsigned int level)
{
  return (level >= _minLevel) && (level <= _maxLevel);
}

void LogMessageHandler::handle(const LogMessage& logMessage)
{
  if (_logMessageListeners.find(logMessage.level())
      != _logMessageListeners.end()) {
    for (auto& logMsgListener : _logMessageListeners[logMessage.level()]) {
      (*logMsgListener)(LogMessage(logMessage));
    }
  }
}

//BABYLON::Logger& Logger::Instance()
//{
//  // Since it's a static variable, if the class has already been created,
//  // It won't be created again.
//  // And it **is** thread-safe in C++11.
//  static Logger loggerInstance;
//
//  // Return a reference to our instance.
//  return loggerInstance;
//}

Logger & LoggerInstance()
{
  // Since it's a static variable, if the class has already been created,
  // It won't be created again.
  // And it **is** thread-safe in C++11.
  static Logger loggerInstance;

  // Return a reference to our instance.
  return loggerInstance;
}


Logger::Logger() = default;

Logger::~Logger()
{
  // Cleanly shutting down log message handler
  _impl._logMessageListeners.clear();
}

LogMessage Logger::CreateMessage(unsigned int level, std::string context,
                                 char const* file, int lineNumber,
                                 char const* func, char const* prettyFunc)
{
  LogMessage logMessage{level, context};
  logMessage.setFile(file);
  logMessage.setLineNumber(lineNumber);
  logMessage.setFunction(func);
  logMessage.setPrettyFunction(prettyFunc);
  return logMessage;
}

void Logger::log(const LogMessage& msg)
{
  _impl.handle(msg);
}

bool Logger::takes(unsigned int level)
{
  return _impl.takes(level);
}

bool Logger::isSubscribed(unsigned int level,
                          LogMessageListener& logMsgListener)
{
  bool subscribed = false;
  if (_impl._logMessageListeners.find(level)
      != _impl._logMessageListeners.end()) {
    auto& _logMsgListenersLvl = _impl._logMessageListeners[level];
    auto it = std::find(_logMsgListenersLvl.begin(), _logMsgListenersLvl.end(),
                        &logMsgListener);
    subscribed = (it != _logMsgListenersLvl.end());
  }
  return subscribed;
}

void Logger::registerLogMessageListener(LogMessageListener& logMsgListener)
{
  for (auto& keyVal : _impl._logMessageListeners) {
    auto& _logMsgListenersLvl = _impl._logMessageListeners[keyVal.first];
    auto it = std::find(_logMsgListenersLvl.begin(), _logMsgListenersLvl.end(),
                        &logMsgListener);
    if (it == _logMsgListenersLvl.end()) {
      auto l = &logMsgListener;
      _logMsgListenersLvl.emplace_back(l);
    }
  }
}

void Logger::unregisterLogMessageListener(
  const LogMessageListener& logMsgListener)
{
  for (const auto& keyVal : _impl._logMessageListeners) {
    auto& _logMsgListenersLvl = _impl._logMessageListeners[keyVal.first];
    auto it = std::find(_logMsgListenersLvl.begin(), _logMsgListenersLvl.end(),
                        &logMsgListener);
    if (it != _logMsgListenersLvl.end()) {
      _logMsgListenersLvl.erase(it);
    }
  }
}

void Logger::registerLogMessageListener(unsigned int level,
                                        LogMessageListener& logMsgListener)
{
  if (_impl.takes(level)) {
    if (_impl._logMessageListeners.find(level)
        != _impl._logMessageListeners.end()) {
      auto& _logMsgListenersLvl = _impl._logMessageListeners[level];
      auto l                    = &logMsgListener;
      auto it
        = std::find(_logMsgListenersLvl.begin(), _logMsgListenersLvl.end(), l);
      if (it == _logMsgListenersLvl.end()) {
        _impl._logMessageListeners[level].emplace_back(l);
      }
    }
  }
}

void Logger::unregisterLogMessageListener(
  unsigned int level, const LogMessageListener& logMsgListener)
{
  if (_impl.takes(level)) {
    if (_impl._logMessageListeners.find(level)
        != _impl._logMessageListeners.end()) {
      auto& _logMsgListenersLvl = _impl._logMessageListeners[level];
      auto it                   = std::find(_logMsgListenersLvl.begin(),
                          _logMsgListenersLvl.end(), &logMsgListener);
      if (it != _logMsgListenersLvl.end()) {
        _logMsgListenersLvl.erase(it);
      }
    }
  }
} // namespace BABYLON


} // end of namespace BABYLON

