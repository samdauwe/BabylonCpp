#ifndef BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H
#define BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT EffectShadersStore {

public:
  EffectShadersStore();
  ~EffectShadersStore();

  unordered_map_t<string_t, const char*>& shaders();
  const unordered_map_t<string_t, const char*>& shaders() const;

private:
  static unordered_map_t<string_t, const char*> _shaders;

}; // end of class EffectShadersStore

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H
