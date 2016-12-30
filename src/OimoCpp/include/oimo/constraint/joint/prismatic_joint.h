#ifndef OIMO_CONSTRAINT_JOINT_PRISMATIC_JOINT_H
#define OIMO_CONSTRAINT_JOINT_PRISMATIC_JOINT_H

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_config.h>
#include <oimo/math/vec3.h>

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
  float _localAxis1X;
  float _localAxis1Y;
  float _localAxis1Z;
  float _localAxis2X;
  float _localAxis2Y;
  float _localAxis2Z;
  Vec3 _nor;
  Vec3 _tan;
  Vec3 _bin;
  // The translational limit and motor information of the joint.
  LimitMotor* _limitMotor;
  AngularConstraint* _ac;
  Translational3Constraint* _t3;

}; // end of class PrismaticJoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_PRISMATIC_JOINT_H
