#ifndef BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H
#define BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H

#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT EffectIncludesShadersStore {

public:
  EffectIncludesShadersStore();
  ~EffectIncludesShadersStore();

  std::unordered_map<std::string, std::string>& shaders();
  const std::unordered_map<std::string, std::string>& shaders() const;

private:
  static std::unordered_map<std::string, std::string> _shaders;

}; // end of class EffectIncludesShadersStore

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H
