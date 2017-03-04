#ifndef OIMO_CONSTRAINT_JOINT_WHEEL_JOINT_H
#define OIMO_CONSTRAINT_JOINT_WHEEL_JOINT_H

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_config.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

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
  // The first axis in local coordinate system.
  Vec3 _localAxis1;
  // The second axis in local coordinate system.
  Vec3 _localAxis2;
  Vec3 _localAngle1, _localAngle2;
  Vec3 _ax1, _ax2;
  Vec3 _an1, _an2;
  Vec3 _tmp, _nor, _tan, _bin;
  // The translational limit and motor information of the joint.
  std::unique_ptr<LimitMotor> _translationalLimitMotorNor;
  std::unique_ptr<LimitMotor> _translationalLimitMotorBin;
  std::unique_ptr<LimitMotor> _translationalLimitMotor;
  std::unique_ptr<LimitMotor> _rotationalLimitMotorNor;
  std::unique_ptr<LimitMotor> _rotationalLimitMotor1;
  std::unique_ptr<LimitMotor> _rotationalLimitMotor2;
  std::unique_ptr<Translational3Constraint> _t3;
  std::unique_ptr<Rotational3Constraint> _r3;

}; // end of class WheelJoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_WHEEL_JOINT_H
