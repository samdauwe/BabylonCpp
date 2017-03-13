#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTOR_JOINT_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTOR_JOINT_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhysicsImpostorJoint {
  PhysicsImpostor* mainImpostor;
  PhysicsImpostor* connectedImpostor;
  std::shared_ptr<PhysicsJoint> joint;
}; // end of struct PhysicsImpostorJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_IMPOSTOR_JOINT_H
