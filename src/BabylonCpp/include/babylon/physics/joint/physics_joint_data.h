#ifndef BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_DATA_H
#define BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_DATA_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhysicsJointData {
  // Important for some engines, optional!
  Vector3 mainPivot;
  Vector3 connectedPivot;
  Vector3 mainAxis;
  Vector3 connectedAxis;
  bool collision;
  // Native Oimo/Cannon/Energy data
  std::string nativeParams;
}; // end of struct PhysicsJointData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_DATA_H
