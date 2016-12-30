#ifndef BABYLON_CORE_PROFILING_MEMORY_H
#define BABYLON_CORE_PROFILING_MEMORY_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Memory {

public:
  static size_t GetMemorySize();
  static size_t GetPeakRSS();
  static size_t GetCurrentRSS();

}; // end of class Memory

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_PROFILING_MEMORY_H
