#ifndef OIMO_CONSTRAINT_JOINT_BASE_TRANSLATION_CONSTRAINT_H
#define OIMO_CONSTRAINT_JOINT_BASE_TRANSLATION_CONSTRAINT_H

#include <oimo/constraint/constraint.h>
#include <oimo/math/mat33.h>
#include <oimo/math/vec3.h>

namespace OIMO {

class Joint;
class LimitMotor;
class RigidBody;

/**
 * @brief A translational constraint for various joints.
 */
class TranslationalConstraint : public Constraint {

public:
  TranslationalConstraint(Joint* joint, LimitMotor* limitMotor);
  ~TranslationalConstraint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;

private:
  float _cfm;
  float _m1, _m2;
  float _i1e00, _i1e01, _i1e02;
  float _i1e10, _i1e11, _i1e12;
  float _i1e20, _i1e21, _i1e22;
  float _i2e00, _i2e01, _i2e02;
  float _i2e10, _i2e11, _i2e12;
  float _i2e20, _i2e21, _i2e22;
  float _motorDenom;
  float _invMotorDenom;
  float _invDenom;
  float _ax, _ay, _az;
  float _r1x, _r1y, _r1z;
  float _r2x, _r2y, _r2z;
  float _t1x, _t1y, _t1z;
  float _t2x, _t2y, _t2z;
  float _l1x, _l1y, _l1z;
  float _l2x, _l2y, _l2z;
  float _a1x, _a1y, _a1z;
  float _a2x, _a2y, _a2z;
  float _lowerLimit, _upperLimit;
  float _limitVelocity;
  int _limitState; // -1: at lower, 0: locked, 1: at upper, 2: free
  bool _enableMotor;
  float _motorSpeed;
  float _maxMotorForce;
  float _maxMotorImpulse;
  LimitMotor* _limitMotor;
  RigidBody *_b1, *_b2;
  Vec3 _p1, _p2;
  Vec3 _r1, _r2;
  Vec3 _l1, _l2;
  Vec3 _a1, _a2;
  Mat33 _i1, _i2;
  float _limitImpulse;
  float _motorImpulse;

}; // end of class TranslationalConstraint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_BASE_TRANSLATION_CONSTRAINT_H