#ifndef BABYLON_PHYSICS_JOINT_IMOTOR_ENABLED_JOINT_H
#define BABYLON_PHYSICS_JOINT_IMOTOR_ENABLED_JOINT_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class PhysicsJoint;

struct BABYLON_SHARED_EXPORT IMotorEnabledJoint {
  PhysicsJoint* physicsJoint;
  virtual void setMotor(float force, float maxForce,
                        unsigned int motorIndex = 0)
    = 0;
  virtual void setLimit(float upperLimit, float lowerLimit,
                        unsigned int motorIndex = 0)
    = 0;

}; // end of struct IMotorEnabledJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_IMOTOR_ENABLED_JOINT_H
