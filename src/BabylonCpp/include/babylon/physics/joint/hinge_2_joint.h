#ifndef BABYLON_PHYSICS_JOINT_HINGE_2_JOINT_H
#define BABYLON_PHYSICS_JOINT_HINGE_2_JOINT_H

#include <babylon/babylon_global.h>
#include <babylon/physics/joint/motor_enabled_joint.h>

namespace BABYLON {

/**
 * @brief This class represents a dual hinge physics joint (same as wheel
 * joint).
 */
class BABYLON_SHARED_EXPORT Hinge2Joint : public MotorEnabledJoint {

public:
  Hinge2Joint(const PhysicsJointData& jointData);
  ~Hinge2Joint();

  /**
   * Set the motor values.
   * Attention, this function is plugin specific. Engines won't react 100%
   * the same.
   * @param {number} force the force to apply
   * @param {number} maxForce max force for this motor.
   * @param {motorIndex} the motor's index, 0 or 1.
   */
  void setMotor(float force, float maxForce, unsigned int motorIndex = 0);

  /**
   * Set the motor limits.
   * Attention, this function is plugin specific. Engines won't react 100% the
   * same.
   * @param {number} upperLimit the upper limit
   * @param {number} lowerLimit lower limit
   * @param {motorIndex} the motor's index, 0 or 1.
   */
  void setLimit(float upperLimit, float lowerLimit,
                unsigned int motorIndex = 0);

}; // end of class Hinge2Joint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_HINGE_2_JOINT_H
