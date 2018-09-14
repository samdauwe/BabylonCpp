#ifndef BABYLON_CORE_LOGGING_LOG_LEVELS_H
#define BABYLON_CORE_LOGGING_LOG_LEVELS_H

#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT LogLevels {

public:
  /** No logging. **/
  static constexpr unsigned int LEVEL_QUIET = 0;
  /** Application crashes / exceptions. **/
  static constexpr unsigned int LEVEL_ERROR = 1;
  /** Incorrect behavior but the application can continue. **/
  static constexpr unsigned int LEVEL_WARN = 2;
  /** Normal behavior. **/
  static constexpr unsigned int LEVEL_INFO = 3;
  /** Detailed information **/
  static constexpr unsigned int LEVEL_DEBUG = 4;
  /** Begin method X, end method X etc. **/
  static constexpr unsigned int LEVEL_TRACE = 5;
  /** Levels list **/
  static const std::vector<std::pair<unsigned int, std::string>> Levels;

  // Functions
  static std::string ToReadableLevel(unsigned int level);

}; // end of class LogChannel

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_LOGGING_LOG_LEVELS_H
