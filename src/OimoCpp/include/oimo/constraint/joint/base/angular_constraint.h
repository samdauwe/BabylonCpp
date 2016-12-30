#ifndef OIMO_CONSTRAINT_JOINT_BASE_ANGULAR_CONSTRAINT_H
#define OIMO_CONSTRAINT_JOINT_BASE_ANGULAR_CONSTRAINT_H

#include <oimo/constraint/constraint.h>
#include <oimo/math/mat33.h>
#include <oimo/math/quat.h>
#include <oimo/math/vec3.h>

namespace OIMO {

class Joint;
class RigidBody;

/**
 * @brief An angular constraint for all axes for various joints.
 */
class AngularConstraint : public Constraint {

public:
  AngularConstraint(Joint* joint, const Quat& targetOrientation);
  ~AngularConstraint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;

private:
  Joint* _joint;
  Quat _targetOrientation;
  Quat _relativeOrientation;
  Mat33 _ii1, _ii2;
  Mat33 _dd;
  Vec3 _vel;
  Vec3 _imp;
  Vec3 _rn0, _rn1, _rn2;
  RigidBody *_b1, *_b2;
  Vec3 _a1, _a2;
  Mat33 _i1, _i2;

}; // end of class AngularConstraint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_BASE_ANGULAR_CONSTRAINT_H
