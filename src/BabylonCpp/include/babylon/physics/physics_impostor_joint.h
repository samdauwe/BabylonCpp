#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTOR_JOINT_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTOR_JOINT_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class PhysicsImpostor;
class PhysicsJoint;

/**
 * @brief Interface used to describe a physics joint.
 */
struct BABYLON_SHARED_EXPORT PhysicsImpostorJoint {
  /** Defines the main impostor to which the joint is linked */
  PhysicsImpostor* mainImpostor;
  /** Defines the impostor that is connected to the main impostor using this
   * joint */
  PhysicsImpostor* connectedImpostor;
  /** Defines the joint itself */
  std::shared_ptr<PhysicsJoint> joint;
}; // end of struct PhysicsImpostorJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_IMPOSTOR_JOINT_H
