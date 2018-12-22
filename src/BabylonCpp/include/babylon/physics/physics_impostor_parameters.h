#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H

#include <optional>
#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief The interface for the physics imposter parameters.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT PhysicsImpostorParameters {
  /**
   * The mass of the physics imposter
   */
  std::optional<float> mass = std::nullopt;
  /**
   * The friction of the physics imposter
   */
  std::optional<float> friction = std::nullopt;
  /**
   * The coefficient of restitution of the physics imposter
   */
  std::optional<float> restitution = std::nullopt;
  /**
   * Specifies if the parent should be ignored
   */
  std::optional<bool> ignoreParent = std::nullopt;
  /**
   * Specifies if bi-directional transformations should be disabled
   */
  std::optional<bool> disableBidirectionalTransformation = std::nullopt;
  /**
   * The native options of the physics imposter
   */
  std::unordered_map<std::string, float> nativeOptions;
  /**
   * @brief Hidden
   */
  bool contains(const std::string& key) const;
  /**
   * @brief Hidden
   */
  float operator[](const std::string& key) const;
  /**
   * @brief Hidden
   */
  void setValue(const std::string& key, float value);
}; // end of struct PhysicsImpostorParameters

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
