#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhysicsImpostorParameters {
  float mass        = 0.f;
  float friction    = 0.2f;
  float restitution = 0.2f;
  std::unordered_map<std::string, float> nativeOptions;
  bool contains(const std::string& key) const;
  float operator[](const std::string& key) const;
  void setValue(const std::string& key, float value);
}; // end of struct PhysicsImpostorParameters

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
