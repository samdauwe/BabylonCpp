#ifndef BABYLON_ENGINE_ENGINE_OPTIONS_H
#define BABYLON_ENGINE_ENGINE_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT EngineOptions {
  bool adaptToDeviceRatio    = true;
  bool antialias             = true;
  int limitDeviceRatio       = false;
  bool autoEnableWebVR       = false;
  bool preserveDrawingBuffer = false;
  bool stencil               = true;
}; // end of struct EngineOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ENGINE_OPTIONS_H
