#ifndef BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_DATA_H
#define BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_DATA_H

#include <babylon/babylon_global.h>
#include <babylon/physics/joint/physics_joint_data.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT DistanceJointData : public PhysicsJointData {
  float maxDistance;
}; // end of struct DistanceJointData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_DISTANCE_JOINT_DATA_H
