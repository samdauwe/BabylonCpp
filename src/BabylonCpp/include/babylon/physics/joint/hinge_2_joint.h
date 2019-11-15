#ifndef BABYLON_PHYSICS_JOINT_HINGE_2_JOINT_H
#define BABYLON_PHYSICS_JOINT_HINGE_2_JOINT_H

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/motor_enabled_joint.h>

namespace BABYLON {

/**
 * @brief This class represents a dual hinge physics joint (same as wheel
 * joint).
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT Hinge2Joint : public MotorEnabledJoint {

public:
  /**
   * @brief Initializes the Hinge2-Joint.
   * @param jointData The joint data for the Hinge2-Joint
   */
  Hinge2Joint(const PhysicsJointData& jointData);
  ~Hinge2Joint(); // = default

  /**
   * @brief Set the motor values.
   * Attention, this function is plugin specific. Engines won't react 100%
   * the same.
   * @param {number} targetSpeed the speed the motor is to reach.
   * @param {number} maxForce max force for this motor.
   * @param {motorIndex} the motor's index, 0 or 1.
   */
  void setMotor(float targetSpeed, float maxForce, unsigned int motorIndex = 0);

  /**
   * @brief Set the motor limits.
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
