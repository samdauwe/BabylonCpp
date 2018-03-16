#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhysicsImpostorParameters {
  Nullable<float> mass        = nullptr;
  Nullable<float> friction    = nullptr;
  Nullable<float> restitution = nullptr;
  bool ignoreParent           = false;
  unordered_map_t<string_t, float> nativeOptions;
  bool contains(const string_t& key) const;
  float operator[](const string_t& key) const;
  void setValue(const string_t& key, float value);
}; // end of struct PhysicsImpostorParameters

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
