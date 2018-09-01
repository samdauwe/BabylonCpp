#ifndef BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H
#define BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT EffectIncludesShadersStore {

public:
  EffectIncludesShadersStore();
  ~EffectIncludesShadersStore();

  unordered_map_t<string_t, string_t>& shaders();
  const unordered_map_t<string_t, string_t>& shaders() const;

private:
  static unordered_map_t<string_t, string_t> _shaders;

}; // end of class EffectIncludesShadersStore

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H
