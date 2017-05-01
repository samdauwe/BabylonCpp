#ifndef BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H
#define BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT EffectCreationOptions {
  std::vector<std::string> attributes{};
  std::vector<std::string> uniformsNames{};
  std::vector<std::string> uniformBuffersNames{};
  std::vector<std::string> samplers{};
  MaterialDefines* materialDefines{nullptr};
  std::string defines{""};
  std::unique_ptr<EffectFallbacks> fallbacks{nullptr};
  std::function<void(Effect* effect)> onCompiled{nullptr};
  std::function<void(Effect* effect, const std::string& errors)> onError{
    nullptr};
  std::unordered_map<std::string, unsigned int> indexParameters{};
  unsigned int number{0};
  unsigned int maxSimultaneousLights{4};
}; // end of class EffectCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H
