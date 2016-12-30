#ifndef OIMO_CONSTRAINT_JOINT_BASE_LINEAR_CONSTRAINT_H
#define OIMO_CONSTRAINT_JOINT_BASE_LINEAR_CONSTRAINT_H

#include <oimo/constraint/constraint.h>
#include <oimo/math/mat33.h>
#include <oimo/math/vec3.h>

namespace OIMO {

class Joint;
class RigidBody;

/**
 * @brief A linear constraint for all axes for various joints.
 */
class LinearConstraint : public Constraint {

public:
  LinearConstraint(Joint* joint);
  ~LinearConstraint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;

private:
  float _m1, _m2;
  Mat33 _ii1, _ii2;
  Mat33 _dd;
  float _r1x, _r1y, _r1z;
  float _r2x, _r2y, _r2z;
  float _ax1x, _ax1y, _ax1z;
  float _ay1x, _ay1y, _ay1z;
  float _az1x, _az1y, _az1z;
  float _ax2x, _ax2y, _ax2z;
  float _ay2x, _ay2y, _ay2z;
  float _az2x, _az2y, _az2z;
  float _velx, _vely, _velz;
  Joint* _joint;
  Vec3 _r1, _r2;
  Vec3 _p1, _p2;
  RigidBody *_b1, *_b2;
  Vec3 _l1, _l2;
  Vec3 _a1, _a2;
  Mat33 _i1, _i2;
  float _impx, _impy, _impz;

}; // end of class LinearConstraint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_BASE_LINEAR_CONSTRAINT_H
