#ifndef BABYLON_PHYSICS_JOINT_HINGE_JOINT_H
#define BABYLON_PHYSICS_JOINT_HINGE_JOINT_H

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/motor_enabled_joint.h>

namespace BABYLON {

/**
 * @brief This class represents a single hinge physics joint.
 */
class BABYLON_SHARED_EXPORT HingeJoint : public MotorEnabledJoint {

public:
  HingeJoint(const PhysicsJointData& jointData);
  ~HingeJoint() override;

  /**
   * Set the motor values.
   * Attention, this function is plugin specific. Engines won't react 100% the
   * same.
   * @param {number} force the force to apply
   * @param {number} maxForce max force for this motor.
   */
  void setMotor(float force, float maxForce,
                unsigned int motorIndex = 0) override;

  /**
   * Set the motor's limits.
   * Attention, this function is plugin specific. Engines won't react 100% the
   * same.
   */
  void setLimit(float upperLimit, float lowerLimit,
                unsigned int motorIndex = 0) override;

}; // end of class HingeJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_HINGE_JOINT_H
