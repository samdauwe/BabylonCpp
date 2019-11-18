#include <babylon/core/logging/log_levels.h>

// This file instantiates the constexpr variables of LogLevels.
// It should be included by only one cpp file!
//
// Warning: there is an issue with STL containers and dlls!
// See https://stackoverflow.com/questions/4145605/stdvector-needs-to-have-dll-interface-to-be-used-by-clients-of-class-xt-war)
// For the moment, as a quick (and dirty) hack under windows, 
// this file is included by the "BabylonCpp" lib *AND* the "inspector" lib.

namespace BABYLON {
  constexpr unsigned int LogLevels::LEVEL_QUIET;
  constexpr unsigned int LogLevels::LEVEL_ERROR;
  constexpr unsigned int LogLevels::LEVEL_WARN;
  constexpr unsigned int LogLevels::LEVEL_INFO;
  constexpr unsigned int LogLevels::LEVEL_DEBUG;
  constexpr unsigned int LogLevels::LEVEL_TRACE;

  const std::vector<std::pair<unsigned int, std::string>> LogLevels::Levels
    = { std::make_pair(LogLevels::LEVEL_QUIET, "QUIET"),
       std::make_pair(LogLevels::LEVEL_ERROR, "ERROR"),
       std::make_pair(LogLevels::LEVEL_WARN, "WARN"),
       std::make_pair(LogLevels::LEVEL_INFO, "INFO"),
       std::make_pair(LogLevels::LEVEL_DEBUG, "DEBUG"),
       std::make_pair(LogLevels::LEVEL_TRACE, "TRACE") };
  } // namespace BABYLON
