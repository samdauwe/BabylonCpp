#ifndef BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_H
#define BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_H

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/physics_joint.h>

namespace BABYLON {

struct DistanceJointData;

/**
 * @brief A class representing a physics distance joint.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT DistanceJoint : public PhysicsJoint {

public:
  /**
   * @brief Initializes the Distance-Joint.
   * @param jointData The data for the Distance-Joint
   */
  DistanceJoint(const DistanceJointData& jointData);
  ~DistanceJoint(); // = default

  /**
   * @brief Update the predefined distance.
   * @param maxDistance The maximum preferred distance
   * @param minDistance The minimum preferred distance
   */
  void updateDistance(float maxDistance, float minDistance);

}; // end of class DistanceJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_H
