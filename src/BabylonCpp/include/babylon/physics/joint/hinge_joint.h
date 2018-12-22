#ifndef BABYLON_PHYSICS_JOINT_HINGE_JOINT_H
#define BABYLON_PHYSICS_JOINT_HINGE_JOINT_H

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/motor_enabled_joint.h>

namespace BABYLON {

/**
 * @brief This class represents a single physics Hinge-Joint.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT HingeJoint : public MotorEnabledJoint {

public:
  /**
   * @brief Initializes the Hinge-Joint.
   * @param jointData The joint data for the Hinge-Joint
   */
  HingeJoint(const PhysicsJointData& jointData);
  ~HingeJoint() override;

  /**
   * @brief Set the motor values.
   * Attention, this function is plugin specific. Engines won't react 100% the
   * same.
   * @param {number} force the force to apply
   * @param {number} maxForce max force for this motor.
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

}; // end of class HingeJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_HINGE_JOINT_H
