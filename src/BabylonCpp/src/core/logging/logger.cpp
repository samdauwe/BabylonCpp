#include <babylon/core/logging/logger.h>

#include <babylon/core/future.h>
#include <babylon/core/logging/log_message.h>

namespace BABYLON {

LogMessageHandler::LogMessageHandler()
    : _bg(Active::createActive())
    , _minLevel{LogLevels::LEVEL_QUIET}
    , _maxLevel{LogLevels::LEVEL_TRACE}
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
    std::vector<LogMessageListener*> _logMsgListenersLvl
      = _logMessageListeners[logMessage.level()];
    for (auto& logMsgListener : _logMsgListenersLvl) {
      (*logMsgListener)(LogMessage(logMessage));
    }
  }
}

Logger::Logger()
{
}

Logger::~Logger()
{
  using namespace std::literals;
  // Cleanly shutting down log message handler
  auto bg_clear_listeners_call = [this] { _impl._logMessageListeners.clear(); };
  auto token_cleared = spawn_task(bg_clear_listeners_call, _impl._bg.get());
  token_cleared.wait_for(1s);
  _impl._bg.reset(nullptr);
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
  _impl._bg->send([this, msg] { _impl.handle(msg); });
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
    std::vector<LogMessageListener*>& _logMsgListenersLvl
      = _impl._logMessageListeners[level];
    auto it    = std::find(_logMsgListenersLvl.begin(),
                          _logMsgListenersLvl.end(), &logMsgListener);
    subscribed = (it != _logMsgListenersLvl.end());
  }
  return subscribed;
}

void Logger::registerLogMessageListener(LogMessageListener& logMsgListener)
{
  auto bgAddMsgListenercall = [this, &logMsgListener] {
    for (auto& keyVal : _impl._logMessageListeners) {
      std::vector<LogMessageListener*>& _logMsgListenersLvl
        = _impl._logMessageListeners[keyVal.first];
      auto it = std::find(_logMsgListenersLvl.begin(),
                            _logMsgListenersLvl.end(), &logMsgListener);
      if (it == _logMsgListenersLvl.end()) {
        LogMessageListener* l = &logMsgListener;
        _logMsgListenersLvl.emplace_back(l);
      }
    }
  };
  auto token_done = spawn_task(bgAddMsgListenercall, _impl._bg.get());
  token_done.wait();
}

void Logger::unregisterLogMessageListener(
  const LogMessageListener& logMsgListener)
{
  auto bgRemoveMsgListenercall = [this, logMsgListener] {
    for (const auto& keyVal : _impl._logMessageListeners) {
      std::vector<LogMessageListener*>& _logMsgListenersLvl
        = _impl._logMessageListeners[keyVal.first];
      auto it = std::find(_logMsgListenersLvl.begin(),
                            _logMsgListenersLvl.end(), &logMsgListener);
      if (it != _logMsgListenersLvl.end()) {
        _logMsgListenersLvl.erase(it);
      }
    }
  };
  auto token_done = spawn_task(bgRemoveMsgListenercall, _impl._bg.get());
  token_done.wait();
}

void Logger::registerLogMessageListener(unsigned int level,
                                        LogMessageListener& logMsgListener)
{
  if (_impl.takes(level)) {
    auto bgAddMsgListenercall = [this, &level, &logMsgListener] {
      if (_impl._logMessageListeners.find(level)
          != _impl._logMessageListeners.end()) {
        std::vector<LogMessageListener*>& _logMsgListenersLvl
          = _impl._logMessageListeners[level];
        LogMessageListener* l = &logMsgListener;
        auto it               = std::find(_logMsgListenersLvl.begin(),
                              _logMsgListenersLvl.end(), l);
        if (it == _logMsgListenersLvl.end()) {
          _impl._logMessageListeners[level].emplace_back(l);
        }
      }
    };
    auto token_done = spawn_task(bgAddMsgListenercall, _impl._bg.get());
    token_done.wait();
  }
}

void Logger::unregisterLogMessageListener(
  unsigned int level, const LogMessageListener& logMsgListener)
{
  if (_impl.takes(level)) {
    auto bgRemoveMsgListenercall = [this, &level, &logMsgListener] {
      if (_impl._logMessageListeners.find(level)
          != _impl._logMessageListeners.end()) {
        std::vector<LogMessageListener*>& _logMsgListenersLvl
          = _impl._logMessageListeners[level];
        auto it = std::find(_logMsgListenersLvl.begin(),
                              _logMsgListenersLvl.end(), &logMsgListener);
        if (it != _logMsgListenersLvl.end()) {
          _logMsgListenersLvl.erase(it);
        }
      }
    };
    auto token_done = spawn_task(bgRemoveMsgListenercall, _impl._bg.get());
    token_done.wait();
  }
}

} // end of namespace BABYLON
