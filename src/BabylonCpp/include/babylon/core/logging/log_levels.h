#ifndef BABYLON_CORE_LOGGING_LOG_LEVELS_H
#define BABYLON_CORE_LOGGING_LOG_LEVELS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT LogLevels {

public:
  /** No logging. **/
  static const unsigned int LEVEL_QUIET;
  /** Application crashes / exceptions. **/
  static const unsigned int LEVEL_ERROR;
  /** Incorrect behavior but the application can continue. **/
  static const unsigned int LEVEL_WARN;
  /** Normal behavior. **/
  static const unsigned int LEVEL_INFO;
  /** Detailed information **/
  static const unsigned int LEVEL_DEBUG;
  /** Begin method X, end method X etc. **/
  static const unsigned int LEVEL_TRACE;
  /** Levels list **/
  static const vector_t<pair_t<unsigned int, string_t>> Levels;

  // Functions
  static string_t ToReadableLevel(unsigned int level);

}; // end of class LogChannel

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_LOGGING_LOG_LEVELS_H
