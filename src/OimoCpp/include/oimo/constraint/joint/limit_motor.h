#ifndef OIMO_CONSTRAINT_JOINT_LIMIT_MOTOR_H
#define OIMO_CONSTRAINT_JOINT_LIMIT_MOTOR_H

#include <memory.h>
#include <oimo/math/vec3.h>

namespace OIMO {

/**
 * @brief An information of limit and motor.
 */
class LimitMotor {

public:
  LimitMotor(const Vec3& axis, bool fixed = false);
  LimitMotor(const LimitMotor& lm);
  LimitMotor(LimitMotor&& lm);
  ~LimitMotor();
  LimitMotor& operator=(const LimitMotor& lm);
  LimitMotor& operator=(LimitMotor&& lm);
  LimitMotor clone() const;
  std::unique_ptr<LimitMotor> cloneToNewObject() const;

  /**
   * Set limit data into this constraint.
   * @param   lowerLimit
   * @param   upperLimit
   */
  void setLimit(float lowerLimit, float upperLimit);

  /**
   * Set motor data into this constraint.
   * @param   motorSpeed
   * @param   maxMotorForce
   */
  void setMotor(float motorSpeed, float maxMotorForce);

  /**
   * Set spring data into this constraint.
   * @param   frequency
   * @param   dampingRatio
   */
  void setSpring(float frequency, float dampingRatio);

public:
  // The axis of the constraint.
  Vec3 axis;
  // The current angle for rotational constraints.
  float angle;
  // The lower limit. Set lower > upper to disable
  float lowerLimit;
  //  The upper limit. Set lower > upper to disable.
  float upperLimit;
  // The target motor speed.
  float motorSpeed;
  // The maximum motor force or torque. Set 0 to disable.
  float maxMotorForce;
  // The frequency of the spring. Set 0 to disable.
  float frequency;
  // The damping ratio of the spring. Set 0 for no damping, 1 for critical
  // damping.
  float dampingRatio;

}; // end of class LimitMotor

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_LIMIT_MOTOR_H
