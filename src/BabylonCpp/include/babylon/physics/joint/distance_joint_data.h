#ifndef BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_DATA_H
#define BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_DATA_H

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/physics_joint_data.h>

namespace BABYLON {

/**
 * @brief Joint data for a Distance-Joint.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT DistanceJointData : public PhysicsJointData {
  /**
   * Max distance the 2 joint objects can be apart
   */
  float maxDistance;
  // Oimo - minDistance
  // Cannon - maxForce
}; // end of struct DistanceJointData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_DATA_H
