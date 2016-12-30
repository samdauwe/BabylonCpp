#ifndef OIMO_CONSTRAINT_JOINT_BASE_TRANSLATION_3_CONSTRAINT_H
#define OIMO_CONSTRAINT_JOINT_BASE_TRANSLATION_3_CONSTRAINT_H

#include <oimo/constraint/constraint.h>
#include <oimo/math/mat33.h>
#include <oimo/math/vec3.h>

namespace OIMO {

class Joint;
class LimitMotor;
class RigidBody;

/**
 * @brief A three-axis translational constraint for various joints.
 */
class Translational3Constraint : public Constraint {

public:
  Translational3Constraint(Joint* joint, LimitMotor* limitMotor1,
                           LimitMotor* limitMotor2, LimitMotor* limitMotor3);
  ~Translational3Constraint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;

public:
  float weight;

private:
  float _m1, _m2;
  float _i1e00, _i1e01, _i1e02;
  float _i1e10, _i1e11, _i1e12;
  float _i1e20, _i1e21, _i1e22;
  float _i2e00, _i2e01, _i2e02;
  float _i2e10, _i2e11, _i2e12;
  float _i2e20, _i2e21, _i2e22;
  float _ax1, _ay1, _az1;
  float _ax2, _ay2, _az2;
  float _ax3, _ay3, _az3;
  float _r1x, _r1y, _r1z;
  float _r2x, _r2y, _r2z;
  float _t1x1, _t1y1, _t1z1; // jacobians
  float _t2x1, _t2y1, _t2z1;
  float _l1x1, _l1y1, _l1z1;
  float _l2x1, _l2y1, _l2z1;
  float _a1x1, _a1y1, _a1z1;
  float _a2x1, _a2y1, _a2z1;
  float _t1x2, _t1y2, _t1z2;
  float _t2x2, _t2y2, _t2z2;
  float _l1x2, _l1y2, _l1z2;
  float _l2x2, _l2y2, _l2z2;
  float _a1x2, _a1y2, _a1z2;
  float _a2x2, _a2y2, _a2z2;
  float _t1x3, _t1y3, _t1z3;
  float _t2x3, _t2y3, _t2z3;
  float _l1x3, _l1y3, _l1z3;
  float _l2x3, _l2y3, _l2z3;
  float _a1x3, _a1y3, _a1z3;
  float _a2x3, _a2y3, _a2z3;
  float _lowerLimit1, _upperLimit1;
  float _limitVelocity1;
  int _limitState1; // -1: at lower, 0: locked, 1: at upper, 2: unlimited
  bool _enableMotor1;
  float _motorSpeed1;
  float _maxMotorForce1;
  float _maxMotorImpulse1;
  float _lowerLimit2;
  float _upperLimit2;
  float _limitVelocity2;
  int _limitState2; // -1: at lower, 0: locked, 1: at upper, 2: unlimited
  bool _enableMotor2;
  float _motorSpeed2;
  float _maxMotorForce2;
  float _maxMotorImpulse2;
  float _lowerLimit3;
  float _upperLimit3;
  float _limitVelocity3;
  int _limitState3; // -1: at lower, 0: locked, 1: at upper, 2: unlimited
  bool _enableMotor3;
  float _motorSpeed3;
  float _maxMotorForce3;
  float _maxMotorImpulse3;
  float _k00, _k01, _k02; // K = J*M*JT
  float _k10, _k11, _k12;
  float _k20, _k21, _k22;
  float _kv00, _kv11, _kv22; // diagonals without CFMs
  float _dv00, _dv11, _dv22; // ...inverted
  float _d00, _d01, _d02;    // K^-1
  float _d10, _d11, _d12;
  float _d20, _d21, _d22;
  LimitMotor *_limitMotor1, *_limitMotor2, *_limitMotor3;
  RigidBody *_b1, *_b2;
  Vec3 _p1, _p2;
  Vec3 _r1, _r2;
  Vec3 _l1, _l2;
  Vec3 _a1, _a2;
  Mat33 _i1, _i2;
  float _limitImpulse1;
  float _motorImpulse1;
  float _limitImpulse2;
  float _motorImpulse2;
  float _limitImpulse3;
  float _motorImpulse3;
  float _cfm1, _cfm2, _cfm3; // Constraint Force Mixing

}; // end of class Translational3Constraint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_BASE_TRANSLATION_3_CONSTRAINT_H
