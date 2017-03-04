#ifndef OIMO_CONSTRAINT_JOINT_SLIDER_JOINT_H
#define OIMO_CONSTRAINT_JOINT_SLIDER_JOINT_H

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_config.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

class LimitMotor;
class Rotational3Constraint;
class Translational3Constraint;

/**
 * @brief A slider joint allows for relative translation and relative rotation
 * between two rigid bodies along the axis.
 * along the axis.
 */
class SliderJoint : public Joint {

public:
  SliderJoint(const JointConfig& config, float lowerTranslation,
              float upperTranslation);
  ~SliderJoint();

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
  // The limit and motor for the rotation
  std::unique_ptr<LimitMotor> _rotationalLimitMotor;
  std::unique_ptr<LimitMotor> _rotationalLimitMotorTan;
  std::unique_ptr<LimitMotor> _rotationalLimitMotorBin;
  std::unique_ptr<Rotational3Constraint> _r3;
  // The limit and motor for the translation.
  std::unique_ptr<LimitMotor> _translationalLimitMotor;
  std::unique_ptr<LimitMotor> _translationalLimitMotorTan;
  std::unique_ptr<LimitMotor> _translationalLimitMotorBin;
  std::unique_ptr<Translational3Constraint> _t3;

}; // end of class SliderJoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_SLIDER_JOINT_H
