#ifndef BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_H
#define BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_H

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/physics_joint.h>

namespace BABYLON {

struct DistanceJointData;

/**
 * @brief A class representing a physics distance joint.
 */
class BABYLON_SHARED_EXPORT DistanceJoint : public PhysicsJoint {

public:
  DistanceJoint(const DistanceJointData& jointData);
  ~DistanceJoint();

  /**
   * Update the predefined distance.
   */
  void updateDistance(float maxDistance, float minDistance);

}; // end of class DistanceJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_H
