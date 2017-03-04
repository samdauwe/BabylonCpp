#ifndef OIMO_CONSTRAINT_JOINT_HINGE_JOINT_H
#define OIMO_CONSTRAINT_JOINT_HINGE_JOINT_H

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_config.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

class LimitMotor;
class LinearConstraint;
class Rotational3Constraint;

/**
 * @brief A hinge joint allows only for relative rotation of rigid bodies along
 * the axis.
 */
class HingeJoint : public Joint {

public:
  HingeJoint(const JointConfig& config, float lowerAngleLimit,
             float upperAngleLimit);
  ~HingeJoint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;
  void postSolve() override;

private:
  // The axis in the first body's coordinate system.
  Vec3 _localAxis1;
  // The axis in the second body's coordinate system.
  Vec3 _localAxis2;
  // Angle axis 1
  Vec3 _localAngle1;
  // Angle axis 2
  Vec3 _localAngle2;
  Vec3 _nor;
  Vec3 _tan;
  Vec3 _bin;
  Vec3 _ax1, _ax2;
  Vec3 _an1, _an2;
  std::unique_ptr<LimitMotor> _limitMotor, _limitMotorTan, _limitMotorBin;
  std::unique_ptr<LinearConstraint> _lc;
  std::unique_ptr<Rotational3Constraint> _r3;

}; // end of class HingeJoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_HINGE_JOINT_H
