#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_IPOST_PROCESS_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_IPOST_PROCESS_H

#include <functional>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Effect;
class Engine;

namespace Extensions {

struct BABYLON_SHARED_EXPORT IPostProcess {
  unsigned int samplingMode;
  Engine* engine;
  bool reusable;
  std::string defines;
  std::function<void(Effect* effect)> onApply;
}; // end of struct IPostProcess

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_IPOST_PROCESS_H
