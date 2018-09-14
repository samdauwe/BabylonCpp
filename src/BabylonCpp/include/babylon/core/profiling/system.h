#ifndef BABYLON_CORE_PROFILING_SYSTEM_H
#define BABYLON_CORE_PROFILING_SYSTEM_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT System {

public:
  static int GetCurrentPID();
  static const char* GetOperatingSystem();
  static const char* GetProcessorArch();

}; // end of class System

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_PROFILING_SYSTEM_H
