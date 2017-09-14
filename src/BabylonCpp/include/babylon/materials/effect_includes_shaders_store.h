#ifndef BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H
#define BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT EffectIncludesShadersStore {
  static std::unordered_map<string_t, const char*> Shaders;
}; // end of struct EffectIncludesShadersStore

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H
