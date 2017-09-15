#ifndef BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H
#define BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H

#include <babylon/babylon_global.h>
#include <babylon/materials/effect_fallbacks.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT EffectCreationOptions {
  vector_t<string_t> attributes{};
  vector_t<string_t> uniformsNames{};
  vector_t<string_t> uniformBuffersNames{};
  vector_t<string_t> samplers{};
  MaterialDefines* materialDefines{nullptr};
  string_t defines{""};
  unique_ptr_t<EffectFallbacks> fallbacks{nullptr};
  ::std::function<void(Effect* effect)> onCompiled{nullptr};
  ::std::function<void(Effect* effect, const string_t& errors)> onError{
    nullptr};
  unordered_map_t<string_t, unsigned int> indexParameters{};
  unsigned int number{0};
  unsigned int maxSimultaneousLights{4};
}; // end of class EffectCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H
