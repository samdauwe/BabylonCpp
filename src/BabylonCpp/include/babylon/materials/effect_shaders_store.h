#ifndef BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H
#define BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H

#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT EffectShadersStore {

public:
  EffectShadersStore();
  ~EffectShadersStore();

  std::unordered_map<std::string, std::string>& shaders();
  const std::unordered_map<std::string, std::string>& shaders() const;

private:
  static std::unordered_map<std::string, std::string> _shaders;

}; // end of class EffectShadersStore

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H
