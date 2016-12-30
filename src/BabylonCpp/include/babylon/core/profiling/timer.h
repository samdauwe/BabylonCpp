#ifndef BABYLON_CORE_PROFILING_TIMER_H
#define BABYLON_CORE_PROFILING_TIMER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Timer {

public:
  static double GetRealTime();
  static double GetCPUTime();

}; // end of class Timer

} // end of namespace BABYLON

#endif // BABYLON_CORE_PROFILING_TIMER_H
