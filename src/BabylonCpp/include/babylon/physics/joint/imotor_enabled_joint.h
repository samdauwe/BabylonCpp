#ifndef BABYLON_PHYSICS_JOINT_IMOTOR_ENABLED_JOINT_H
#define BABYLON_PHYSICS_JOINT_IMOTOR_ENABLED_JOINT_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class PhysicsJoint;

/**
 * @brief Interface for a motor enabled joint.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT IMotorEnabledJoint {
  virtual ~IMotorEnabledJoint() = default;
  /**
   * Physics joint
   */
  PhysicsJoint* physicsJoint;
  /**
   * @brief Sets the motor of the motor-enabled joint.
   * @param force The force of the motor
   * @param maxForce The maximum force of the motor
   * @param motorIndex The index of the motor
   */
  virtual void setMotor(float force, float maxForce, unsigned int motorIndex = 0) = 0;
  /**
   * @brief Sets the limit of the motor.
   * @param upperLimit The upper limit of the motor
   * @param lowerLimit The lower limit of the motor
   * @param motorIndex The index of the motor
   */
  virtual void setLimit(float upperLimit, float lowerLimit, unsigned int motorIndex = 0) = 0;

}; // end of struct IMotorEnabledJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_IMOTOR_ENABLED_JOINT_H
