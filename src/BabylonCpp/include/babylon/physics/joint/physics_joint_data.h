#ifndef BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_DATA_H
#define BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_DATA_H

#include <nlohmann/json.hpp>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>
#include <babylon/physics/physics_params.h>

using json = nlohmann::json;

namespace BABYLON {

/**
 * @brief Interface for Physics-Joint data.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT PhysicsJointData {
  // Important for some engines, optional!
  /**
   * The main pivot of the joint
   */
  std::optional<Vector3> mainPivot = std::nullopt;
  /**
   * The connected pivot of the joint
   */
  std::optional<Vector3> connectedPivot = std::nullopt;
  /**
   * The main axis of the joint
   */
  std::optional<Vector3> mainAxis = std::nullopt;
  /**
   * The connected axis of the joint
   */
  std::optional<Vector3> connectedAxis = std::nullopt;
  /**
   * The collision of the joint
   */
  std::optional<bool> collision = std::nullopt;
  /**
   * Native Oimo/Cannon/Energy data
   */
  PhysicsParams nativeParams;
}; // end of struct PhysicsJointData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_DATA_H
