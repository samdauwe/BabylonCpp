#ifndef BABYLON_CORE_LOGGING_LOGGER_H
#define BABYLON_CORE_LOGGING_LOGGER_H

#include <babylon/babylon_global.h>
#include <babylon/core/active.h>
#include <babylon/core/logging/log_levels.h>
#include <babylon/core/logging/log_message.h>

#if _MSC_VER && !__INTEL_COMPILER
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCDNAME__
#endif
#endif

// thread_local doesn't exist before VS2013
// it exists on VS2015
#if !(defined(thread_local)) && defined(_MSC_VER) && _MSC_VER < 1900
#define thread_local __declspec(thread)
#endif

namespace BABYLON {

struct LogMessageHandler {
  using LogMessageListener = delegate_t<void(const LogMessage&)>;

  LogMessageHandler();
  ~LogMessageHandler()                        = default;
  LogMessageHandler(const LogMessageHandler&) = delete;
  LogMessageHandler& operator=(const LogMessageHandler&) = delete;

  bool takes(unsigned int level);
  void handle(const LogMessage& msg);

  unordered_map_t<unsigned int, vector_t<LogMessageListener*>>
    _logMessageListeners;
  unique_ptr_t<Active> _bg;
  unsigned int _minLevel, _maxLevel;
};

class BABYLON_SHARED_EXPORT Logger {

public:
  using LogMessageListener = delegate_t<void(const LogMessage&)>;

public:
  static Logger& Instance()
  {
    // Since it's a static variable, if the class has already been created,
    // It won't be created again.
    // And it **is** thread-safe in C++11.
    static Logger loggerInstance;

    // Return a reference to our instance.
    return loggerInstance;
  }

  // Delete copy and move constructors and assign operators
  Logger(Logger const&) = delete;            // Copy constructor
  Logger(Logger&&)      = delete;            // Move constructor
  Logger& operator=(Logger const&) = delete; // Copy assignment
  Logger& operator=(Logger&&) = delete;      // Move assignment

  static LogMessage CreateMessage(unsigned int level, string_t context,
                                  char const* file, int lineNumber,
                                  char const* func, char const* prettyFunc);
  void log(const LogMessage& logMessage);
  bool takes(unsigned int level);

  bool isSubscribed(unsigned int level, LogMessageListener& logMsgListener);
  void registerLogMessageListener(LogMessageListener& logMsgListener);
  void registerLogMessageListener(unsigned int level,
                                  LogMessageListener& logMsgListener);
  void unregisterLogMessageListener(const LogMessageListener& logMsgListener);
  void unregisterLogMessageListener(unsigned int level,
                                    const LogMessageListener& logMsgListener);

protected:
  Logger();
  ~Logger();

private:
  LogMessageHandler _impl;

}; // end of class LogChannel

} // end of namespace BABYLON

#define BABYLON_LOG_MSG(level, context, ...)                                   \
  if (BABYLON::Logger::Instance().takes(level)) {                              \
    std::ostringstream _ctx;                                                   \
    _ctx << context;                                                           \
    BABYLON::LogMessage _logMessage                                            \
      = BABYLON::Logger::CreateMessage(level, _ctx.str(), __FILE__, __LINE__,  \
                                       __FUNCTION__, __PRETTY_FUNCTION__);     \
    _logMessage.write(__VA_ARGS__);                                            \
    BABYLON::Logger::Instance().log(::std::move(_logMessage));                 \
  }

#define BABYLON_LOGF_MSG(level, context, printf_like_message, ...)             \
  if (BABYLON::Logger::Instance().takes(level)) {                              \
    std::ostringstream _ctx;                                                   \
    _ctx << context;                                                           \
    BABYLON::LogMessage _logMessage                                            \
      = BABYLON::Logger::CreateMessage(level, _ctx.str(), __FILE__, __LINE__,  \
                                       __FUNCTION__, __PRETTY_FUNCTION__);     \
    _logMessage.writef(printf_like_message, __VA_ARGS__);                      \
    BABYLON::Logger::Instance().log(::std::move(_logMessage));                 \
  }

// -- Default API syntax with variadic input parameters --
#define BABYLON_LOG_ERROR(context, ...)                                        \
  BABYLON_LOG_MSG(BABYLON::LogLevels::LEVEL_ERROR, context, __VA_ARGS__)
#define BABYLON_LOG_WARN(context, ...)                                         \
  BABYLON_LOG_MSG(BABYLON::LogLevels::LEVEL_WARN, context, __VA_ARGS__)
#define BABYLON_LOG_INFO(context, ...)                                         \
  BABYLON_LOG_MSG(BABYLON::LogLevels::LEVEL_INFO, context, __VA_ARGS__)
#define BABYLON_LOG_DEBUG(context, ...)                                        \
  BABYLON_LOG_MSG(BABYLON::LogLevels::LEVEL_DEBUG, context, __VA_ARGS__)

// -- printf-like API syntax with variadic input parameters --
#define BABYLON_LOGF_ERROR(context, printf_like_message, ...)                  \
  BABYLON_LOGF_MSG(BABYLON::LogLevels::LEVEL_ERROR, context,                   \
                   printf_like_message, __VA_ARGS__)
#define BABYLON_LOGF_WARN(context, printf_like_message, ...)                   \
  BABYLON_LOGF_MSG(BABYLON::LogLevels::LEVEL_WARN, context,                    \
                   printf_like_message, __VA_ARGS__)
#define BABYLON_LOGF_INFO(context, printf_like_message, ...)                   \
  BABYLON_LOGF_MSG(BABYLON::LogLevels::LEVEL_INFO, context,                    \
                   printf_like_message, __VA_ARGS__)
#define BABYLON_LOGF_DEBUG(context, printf_like_message, ...)                  \
  BABYLON_LOGF_MSG(BABYLON::LogLevels::LEVEL_DEBUG, context,                   \
                   printf_like_message, __VA_ARGS__)

// -- Conditional log printf syntax --
#define BABYLON_LOG_IF_ERROR(context, boolean_expression, printf_like_message, \
                             ...)                                              \
                                                                               \
  if (true == boolean_expression)                                              \
  BABYLON_LOGF_MSG(BABYLON::LogLevels::LEVEL_ERROR, context,                   \
                   printf_like_message, __VA_ARGS__)
#define BABYLON_LOG_IF_WARN(context, boolean_expression, printf_like_message,  \
                            ...)                                               \
  if (true == boolean_expression)                                              \
  BABYLON_LOGF_MSG(BABYLON::LogLevels::LEVEL_WARN, context,                    \
                   printf_like_message, __VA_ARGS__)
#define BABYLON_LOG_IF_INFO(context, boolean_expression, printf_like_message,  \
                            ...)                                               \
  if (true == boolean_expression)                                              \
  BABYLON_LOGF_MSG(BABYLON::LogLevels::LEVEL_INFO, context,                    \
                   printf_like_message, __VA_ARGS__)
#define BABYLON_LOG_IF_DEBUG(context, boolean_expression, printf_like_message, \
                             ...)                                              \
  if (true == boolean_expression)                                              \
  BABYLON_LOGF_MSG(BABYLON::LogLevels::LEVEL_DEBUG, context,                   \
                   printf_like_message, __VA_ARGS__)

#endif // end of BABYLON_CORE_LOGGING_LOGGER_H
