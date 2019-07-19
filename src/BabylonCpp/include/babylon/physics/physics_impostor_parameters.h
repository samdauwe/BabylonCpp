#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTOR_PARAMETERS_H

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

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
   * The native options of the physics imposter
   */
  std::unordered_map<std::string, float> nativeOptions;
  /**
   * Specifies if the parent should be ignored
   */
  std::optional<bool> ignoreParent = std::nullopt;
  /**
   * Specifies if bi-directional transformations should be disabled
   */
  std::optional<bool> disableBidirectionalTransformation = std::nullopt;
  /**
   * The pressure inside the physics imposter, soft object only
   */
  std::optional<float> pressure = std::nullopt;
  /**
   * The stiffness the physics imposter, soft object only
   */
  std::optional<float> stiffness = std::nullopt;
  /**
   * The number of iterations used in maintaining consistent vertex velocities,
   * soft object only
   */
  std::optional<float> velocityIterations = std::nullopt;
  /**
   * The number of iterations used in maintaining consistent vertex positions,
   * soft object only
   */
  std::optional<float> positionIterations = std::nullopt;
  /**
   * The number used to fix points on a cloth (0, 1, 2, 4, 8) or rope (0, 1, 2)
   * only 0 None, 1, back left or top, 2, back right or bottom, 4, front left,
   * 8, front right Add to fix multiple points
   */
  std::optional<float> fixedPoints = std::nullopt;
  /**
   * The collision margin around a soft object
   */
  std::optional<float> margin = std::nullopt;
  /**
   * The collision margin around a soft object
   */
  std::optional<float> damping = std::nullopt;
  /**
   * The path for a rope based on an extrusion
   */
  std::vector<Vector3> path;
  /**
   * The shape of an extrusion used for a rope based on an extrusion
   */
  std::vector<Vector3> shape;
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
