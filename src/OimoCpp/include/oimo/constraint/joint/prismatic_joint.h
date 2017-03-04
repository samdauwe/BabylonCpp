#ifndef OIMO_CONSTRAINT_JOINT_PRISMATIC_JOINT_H
#define OIMO_CONSTRAINT_JOINT_PRISMATIC_JOINT_H

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_config.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

class AngularConstraint;
class LimitMotor;
class Translational3Constraint;

/**
 * @brief A prismatic joint allows only for relative translation of rigid bodies
 * along the axis.
 */
class PrismaticJoint : public Joint {

public:
  PrismaticJoint(const JointConfig& config, float lowerTranslation,
                 float upperTranslation);
  ~PrismaticJoint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;
  void postSolve() override;

private:
  // The axis in the first body's coordinate system.
  Vec3 _localAxis1;
  // The axis in the second body's coordinate system.
  Vec3 _localAxis2;
  Vec3 _ax1, _ax2;
  Vec3 _nor, _tan, _bin;
  // The translational limit and motor information of the joint.
  std::unique_ptr<LimitMotor> _limitMotor;
  std::unique_ptr<LimitMotor> _limitMotorTan;
  std::unique_ptr<LimitMotor> _limitMotorBin;
  std::unique_ptr<AngularConstraint> _ac;
  std::unique_ptr<Translational3Constraint> _t3;

}; // end of class PrismaticJoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_PRISMATIC_JOINT_H
