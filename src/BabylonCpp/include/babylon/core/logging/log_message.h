#ifndef BABYLON_CORE_LOGGING_LOG_MESSAGE_H
#define BABYLON_CORE_LOGGING_LOG_MESSAGE_H

#include <sstream>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/logging/log_levels.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT LogMessage {

public:
  /// Constructs a message with a certain level.
  /// @param level The level of the message.
  LogMessage(unsigned int level         = LogLevels::LEVEL_QUIET,
             const std::string& context = "");
  LogMessage(LogMessage const& otherLogMessage);
  LogMessage(LogMessage&& otherLogMessage);
  LogMessage& operator=(const LogMessage& otherLogMessage);
  LogMessage& operator=(LogMessage&& otherLogMessage);
  ~LogMessage();

  friend std::ostream& operator<<(std::ostream& os, const LogMessage& logMsg);
  std::string toString() const;

  void clear();
  bool empty();
  unsigned int level() const;
  system_time_point_t timestamp() const;
  std::string getReadableTimestamp() const;
  std::string const& file() const;
  void setFile(char const* file);
  int const& lineNumber() const;
  void setLineNumber(int lineNumber);
  std::string const& threadId() const;
  std::string const& context() const;
  std::string const& function() const;
  void setFunction(char const* func);
  std::string const& prettyFunction() const;
  void setPrettyFunction(char const* prettyFunc);
  std::string message() const;

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
  std::string prettify(char const* pretty_func);

private:
  unsigned int _level;
  system_time_point_t _timestamp;
  std::string _file;
  int _lineNumber;
  std::string _threadId;
  std::string _context;
  std::string _function;
  std::string _prettyFunction;
  std::string _expression;
  std::ostringstream _oss;

}; // end of class LogMessage

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_LOGGING_LOG_MESSAGE_H
