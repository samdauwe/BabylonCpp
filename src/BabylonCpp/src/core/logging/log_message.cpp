#include <babylon/core/logging/log_message.h>
#include <babylon/core/time.h>

#include <cstdarg>

namespace BABYLON {

LogMessage::LogMessage(unsigned int lvl, const std::string& ctx)
    : _level{lvl}, _context{ctx}
{
  // Set timestamp
  _timestamp = Time::systemTimepointNow();
  // Set thread id
  std::ostringstream ss;
  ss << ::std::hex << ::std::this_thread::get_id();
  _threadId = ss.str();
}

LogMessage::LogMessage(const LogMessage& otherLogMessage)
    : _level{otherLogMessage._level}
    , _timestamp{otherLogMessage._timestamp}
    , _file{otherLogMessage._file}
    , _lineNumber{otherLogMessage._lineNumber}
    , _threadId{otherLogMessage._threadId}
    , _context{otherLogMessage._context}
    , _function{otherLogMessage._function}
    , _prettyFunction{otherLogMessage._prettyFunction}
{
  _oss << otherLogMessage.message();
}

LogMessage::LogMessage(LogMessage&& otherLogMessage)
{
  *this = ::std::move(otherLogMessage);
}

LogMessage& LogMessage::operator=(const LogMessage& otherLogMessage)
{
  if (&otherLogMessage != this) {
    _level          = otherLogMessage._level;
    _timestamp      = otherLogMessage._timestamp;
    _file           = otherLogMessage._file;
    _lineNumber     = otherLogMessage._lineNumber;
    _threadId       = otherLogMessage._threadId;
    _context        = otherLogMessage._context;
    _function       = otherLogMessage._function;
    _prettyFunction = otherLogMessage._prettyFunction;
    _oss.clear();
    _oss << otherLogMessage._oss.str();
  }

  return *this;
}

LogMessage& LogMessage::operator=(LogMessage&& otherLogMessage)
{
  if (&otherLogMessage != this) {
    _level          = ::std::move(otherLogMessage._level);
    _timestamp      = ::std::move(otherLogMessage._timestamp);
    _file           = ::std::move(otherLogMessage._file);
    _lineNumber     = ::std::move(otherLogMessage._lineNumber);
    _threadId       = ::std::move(otherLogMessage._threadId);
    _context        = ::std::move(otherLogMessage._context);
    _function       = ::std::move(otherLogMessage._function);
    _prettyFunction = ::std::move(otherLogMessage._prettyFunction);
    _oss.clear();
    _oss << otherLogMessage._oss.str();
    otherLogMessage._oss.clear();
  }

  return *this;
}

LogMessage::~LogMessage()
{
}

std::ostream& operator<<(std::ostream& os, const LogMessage& logMessage)
{
  os << '[' << Time::toIso8601Ms(logMessage.timestamp()) << "] "
     << "[" << ::std::setw(5) << LogLevels::ToReadableLevel(logMessage.level())
     << "] "
     << "[" << logMessage.context() << "::" << logMessage.function() << "] "
     << "| " << logMessage.message();
  // Add file and line number for traces
  if (logMessage.level() == LogLevels::LEVEL_TRACE) {
    os << " (" << logMessage.file() << ":" << logMessage.lineNumber() << ")";
  }
  return os;
}

std::string LogMessage::toString() const
{
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

void LogMessage::clear()
{
  _oss.str("");
  _oss.clear();
}

bool LogMessage::empty()
{
  return _oss.tellp() == 0;
}

unsigned int LogMessage::level() const
{
  return _level;
}

system_time_point_t LogMessage::timestamp() const
{
  return _timestamp;
}

std::string LogMessage::getReadableTimestamp() const
{
  return Time::toIso8601Ms(_timestamp);
}

std::string const& LogMessage::file() const
{
  return _file;
}

void LogMessage::setFile(char const* file)
{
  _file = std::string(file);
}

int const& LogMessage::lineNumber() const
{
  return _lineNumber;
}

void LogMessage::setLineNumber(int lineNumber)
{
  _lineNumber = lineNumber;
}

std::string const& LogMessage::threadId() const
{
  return _threadId;
}

std::string const& LogMessage::context() const
{
  return _context;
}

std::string const& LogMessage::function() const
{
  return _function;
}

void LogMessage::setFunction(char const* func)
{
  _function = std::string(func);
}

std::string const& LogMessage::prettyFunction() const
{
  return _prettyFunction;
}

void LogMessage::setPrettyFunction(char const* prettyFunc)
{
  _prettyFunction = prettify(prettyFunc);
}

std::string LogMessage::message() const
{
  return _oss.str();
}

/**
 * capturef, used for "printf" like API in CHECKF, LOGF, LOGF_IF
 * See also for the attribute formatting ref:
 * http://www.codemaestro.com/reviews/18
 */
void LogMessage::writef(const char* printf_like_message, ...)
{
  static const int kMaxMessageSize               = 2048;
  static const std::string kTruncatedWarningText = "[...truncated...]";
  char finished_message[kMaxMessageSize];
  va_list arglist;
  va_start(arglist, printf_like_message);

#if (defined(WIN32) || defined(_WIN32)                                         \
     || defined(__WIN32__) && !defined(__GNUC__))
  const int nbrcharacters
    = vsnprintf_s(finished_message, _countof(finished_message), _TRUNCATE,
                  printf_like_message, arglist);
#else
  const int nbrcharacters = vsnprintf(
    finished_message, sizeof(finished_message), printf_like_message, arglist);
#endif
  va_end(arglist);

  if (nbrcharacters <= 0) {
    _oss << "\n\tERROR LOG MSG NOTIFICATION: Failure to parse successfully "
            "the message";
    _oss << '"' << printf_like_message << '"' << ::std::endl;
  }
  else if (nbrcharacters > kMaxMessageSize) {
    _oss << finished_message << kTruncatedWarningText;
  }
  else {
    _oss << finished_message;
  }
}

std::string LogMessage::prettify(char const* pretty_func)
{
  auto paren     = pretty_func;
  auto c         = pretty_func;
  auto templates = 0;
  while (*c && (*c != ' ' || templates > 0)) {
    switch (*c) {
      default:
        break;
      case 'v': {
        static char const* vi = "virtual";
        auto v                = vi;
        auto i                = c;
        while (*++v == *++i)
          ;
        if (*v == '\0')
          c += 7;
      } break;
      case 't': {
        static char const* tn = "typename";
        auto t                = tn;
        auto i                = c;
        while (*++t == *++i)
          ;
        if (*t == '\0')
          c += 8;
      } break;
      case '<':
        ++templates;
        break;
      case '>':
        --templates;
        break;
      case '(': {
        paren = c;
      } break;
    }
    ++c;
  }
  // No whitespace found, could be (con|des)tructor.
  if (!c)
    return {pretty_func, c};
  if (*paren != '(')
    while (*paren != '(')
      ++paren;
  // The space occurs before the '(', so we have a return type.
  if (++c < paren)
    return {c, paren};
  // If we went beyond the left parenthesis, we're in a (con|des)tructor.
  while (*paren && *paren != '(')
    ++paren;
  return {pretty_func, paren};
}

} // end of namespace BABYLON
