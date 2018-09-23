#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H

#include <optional>
#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhysicsImpostorParameters {
  std::optional<float> mass               = std::nullopt;
  std::optional<float> friction           = std::nullopt;
  std::optional<float> restitution        = std::nullopt;
  bool ignoreParent                       = false;
  bool disableBidirectionalTransformation = false;
  std::unordered_map<std::string, float> nativeOptions;
  bool contains(const std::string& key) const;
  float operator[](const std::string& key) const;
  void setValue(const std::string& key, float value);
}; // end of struct PhysicsImpostorParameters

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
