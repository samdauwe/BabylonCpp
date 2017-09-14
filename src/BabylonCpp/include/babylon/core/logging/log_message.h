#ifndef BABYLON_CORE_LOGGING_LOG_MESSAGE_H
#define BABYLON_CORE_LOGGING_LOG_MESSAGE_H

#include <babylon/babylon_global.h>
#include <babylon/core/logging/log_levels.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT LogMessage {

public:
  /// Constructs a message with a certain level.
  /// @param level The level of the message.
  LogMessage(unsigned int level      = LogLevels::LEVEL_QUIET,
             const string_t& context = "");
  LogMessage(LogMessage const& otherLogMessage);
  LogMessage(LogMessage&& otherLogMessage);
  LogMessage& operator=(const LogMessage& otherLogMessage);
  LogMessage& operator=(LogMessage&& otherLogMessage);
  ~LogMessage();

  friend std::ostream& operator<<(std::ostream& os, const LogMessage& logMsg);
  string_t toString() const;

  void clear();
  bool empty();
  unsigned int level() const;
  system_time_point_t timestamp() const;
  string_t getReadableTimestamp() const;
  string_t const& file() const;
  void setFile(char const* file);
  int const& lineNumber() const;
  void setLineNumber(int lineNumber);
  string_t const& threadId() const;
  string_t const& context() const;
  string_t const& function() const;
  void setFunction(char const* func);
  string_t const& prettyFunction() const;
  void setPrettyFunction(char const* prettyFunc);
  string_t message() const;

  template <typename TF, typename... TR>
  inline void write(TF&& msg, TR&&... rest)
  {
    writeMessages(std::forward<TF>(msg));
    _oss << " ";
    write(std::forward<TR>(rest)...);
  }
  template <typename TF>
  inline void write(TF&& msg)
  {
    writeMessages(std::forward<TF>(msg));
  }
  template <typename TF>
  inline void writeMessages(TF const& msg)
  {
    _oss << msg;
  }
  inline void write()
  {
    // Handle the empty params case
  }

// Use "-Wall" to generate warnings in case of illegal printf format.
//      Ref:  http://www.unixwiz.net/techtips/gnu-c-attributes.html
#ifndef __GNUC__
#define                                                                        \
  __attribute__(x) // Disable 'attributes' if compiler does not support 'em
#endif
  void writef(const char* printf_like_message, ...)
    __attribute__((format(printf, 2, 3)));

private:
  string_t prettify(char const* pretty_func);

private:
  unsigned int _level;
  system_time_point_t _timestamp;
  string_t _file;
  int _lineNumber;
  string_t _threadId;
  string_t _context;
  string_t _function;
  string_t _prettyFunction;
  string_t _expression;
  std::ostringstream _oss;

}; // end of class LogMessage

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_LOGGING_LOG_MESSAGE_H
