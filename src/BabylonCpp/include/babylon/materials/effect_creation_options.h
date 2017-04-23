#ifndef BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H
#define BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT EffectCreationOptions {
  std::vector<std::string> attributes;
  std::vector<std::string> uniformsNames;
  std::vector<std::string> uniformBuffersNames;
  std::vector<std::string> samplers;
  IMaterialDefines* defines;
  EffectFallbacks* fallbacks;
  std::function<void(Effect* effect)> onCompiled;
  std::function<void(Effect* effect, const std::string& errors)> onError;
  std::unordered_map<std::string, unsigned int> indexParameters;
  unsigned int number;
}; // end of class EffectCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H
