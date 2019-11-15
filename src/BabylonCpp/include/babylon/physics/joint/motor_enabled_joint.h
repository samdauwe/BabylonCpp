#ifndef BABYLON_PHYSICS_JOINT_MOTOR_ENABLED_JOINT_H
#define BABYLON_PHYSICS_JOINT_MOTOR_ENABLED_JOINT_H

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/imotor_enabled_joint.h>
#include <babylon/physics/joint/physics_joint.h>

namespace BABYLON {

/**
 * @brief Represents a Motor-Enabled Joint.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT MotorEnabledJoint : public PhysicsJoint,
                                                public IMotorEnabledJoint {

public:
  /**
   * @brief Initializes the Motor-Enabled Joint.
   * @param type The type of the joint
   * @param jointData The physica joint data for the joint
   */
  MotorEnabledJoint(unsigned int jointType, const PhysicsJointData& jointData);
  virtual ~MotorEnabledJoint(); // = default

  /**
   * @brief Set the motor values.
   * Attention, this function is plugin specific. Engines won't react 100% the
   * same.
   * @param force the force to apply
   * @param maxForce max force for this motor.
   */
  void setMotor(float force, float maxForce,
                unsigned int motorIndex = 0) override;

  /**
   * @brief Set the motor's limits.
   * Attention, this function is plugin specific. Engines won't react 100% the
   * same.
   * @param upperLimit The upper limit of the motor
   * @param lowerLimit The lower limit of the motor
   */
  void setLimit(float upperLimit, float lowerLimit,
                unsigned int motorIndex = 0) override;

}; // end of class MotorEnabledJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_MOTOR_ENABLED_JOINT_H
