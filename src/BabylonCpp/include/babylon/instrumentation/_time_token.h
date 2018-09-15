#ifndef BABYLON_INSTRUMENTATION_TIME_TOKEN_H
#define BABYLON_INSTRUMENTATION_TIME_TOKEN_H

#include <babylon/babylon_api.h>

namespace GL {
class IGLQuery;
} // end of namespace GL

namespace BABYLON {

struct BABYLON_SHARED_EXPORT _TimeToken {
  GL::IGLQuery* _startTimeQuery   = nullptr;
  GL::IGLQuery* _endTimeQuery     = nullptr;
  GL::IGLQuery* _timeElapsedQuery = nullptr;
  bool _timeElapsedQueryEnded     = false;
}; // end of struct _TimeToken

} // end of namespace BABYLON

#endif // end of BABYLON_INSTRUMENTATION_TIME_TOKEN_H
