#include <babylon/core/logging/log_levels.h>
#include <babylon/core/logging/log_levels_statics.cpp.h>

namespace BABYLON {

std::string LogLevels::ToReadableLevel(unsigned int level)
{
  switch (level) {
    case LogLevels::LEVEL_QUIET:
      return "QUIET";
    case LogLevels::LEVEL_ERROR:
      return "ERROR";
    case LogLevels::LEVEL_WARN:
      return "WARN";
    case LogLevels::LEVEL_INFO:
      return "INFO";
    case LogLevels::LEVEL_DEBUG:
      return "DEBUG";
    case LogLevels::LEVEL_TRACE:
      return "TRACE";
    default:
      return "QUIET";
  }
}

} // end of namespace BABYLON
