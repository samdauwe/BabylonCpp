#ifndef OIMO_CONSTRAINT_JOINT_WHEEL_JOINT_H
#define OIMO_CONSTRAINT_JOINT_WHEEL_JOINT_H

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_config.h>
#include <oimo/math/vec3.h>

namespace OIMO {

class LimitMotor;
class Rotational3Constraint;
class Translational3Constraint;

/**
 * @brief A wheel joint allows for relative rotation between two rigid bodies
 * along two axes. The wheel joint also allows for relative translation for the
 * suspension.
 */
class WheelJoint : public Joint {

public:
  WheelJoint(const JointConfig& config);
  ~WheelJoint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;
  void postSolve() override;

private:
  float _acosClamp(float cos) const;

private:
  // The first axis in local coordinate system.
  Vec3 _localAxis1;
  // The second axis in local coordinate system.
  Vec3 _localAxis2;
  float _localAxis1X;
  float _localAxis1Y;
  float _localAxis1Z;
  float _localAngAxis1X;
  float _localAngAxis1Y;
  float _localAngAxis1Z;
  float _localAxis2X;
  float _localAxis2Y;
  float _localAxis2Z;
  float _localAngAxis2X;
  float _localAngAxis2Y;
  float _localAngAxis2Z;
  Vec3 _nor;
  Vec3 _tan;
  Vec3 _bin;
  // The translational limit and motor information of the joint.
  LimitMotor* _translationalLimitMotor;
  LimitMotor* _rotationalLimitMotor1;
  LimitMotor* _rotationalLimitMotor2;
  Translational3Constraint* _t3;
  Rotational3Constraint* _r3;

}; // end of class WheelJoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_WHEEL_JOINT_H
