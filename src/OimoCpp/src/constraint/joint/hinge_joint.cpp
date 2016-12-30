#include <oimo/constraint/joint/hinge_joint.h>

#include <cmath>

#include <oimo/constraint/joint/base/linear_constraint.h>
#include <oimo/constraint/joint/base/rotational_3_constraint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/quat.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

HingeJoint::HingeJoint(const JointConfig& config, float lowerAngleLimit,
                       float upperAngleLimit)
    : Joint{config}
    , _localAxis1{config.localAxis1.clone().norm()}
    , _localAxis2{config.localAxis2.clone().norm()}
{
  // Set joint type
  type = Joint::Type::JOINT_HINGE;
  // make angle axis 1
  _localAngle1
    = Vec3(_localAxis1.y * _localAxis1.x - _localAxis1.z * _localAxis1.z,
           -_localAxis1.z * _localAxis1.y - _localAxis1.x * _localAxis1.x,
           _localAxis1.x * _localAxis1.z + _localAxis1.y * _localAxis1.y)
        .norm();

  // make angle axis 2
  Mat33 arc    = Mat33().setQuat(Quat().arc(_localAxis1, _localAxis2));
  _localAngle2 = Vec3().mulMat(arc, _localAngle1);

  // The rotational limit and motor information of the joint.
  _limitMotor             = new LimitMotor(_nor, false);
  _limitMotor->lowerLimit = lowerAngleLimit;
  _limitMotor->upperLimit = upperAngleLimit;

  _lc = new LinearConstraint(this);
  _r3 = new Rotational3Constraint(this, _limitMotor, new LimitMotor(_tan, true),
                                  new LimitMotor(_bin, true));
}

HingeJoint::~HingeJoint()
{
}

void HingeJoint::preSolve(float timeStep, float invTimeStep)
{
  float tmp1X, tmp1Y, tmp1Z, limite;

  updateAnchorPoints();

  _ax1.mulMat(body1->rotation, _localAxis1);
  _ax2.mulMat(body2->rotation, _localAxis2);

  _an1.mulMat(body1->rotation, _localAngle1);
  _an2.mulMat(body2->rotation, _localAngle2);

  _nor
    .set(_ax1.x * body2->inverseMass + _ax2.x * body1->inverseMass,
         _ax1.y * body2->inverseMass + _ax2.y * body1->inverseMass,
         _ax1.z * body2->inverseMass + _ax2.z * body1->inverseMass)
    .norm();

  _tan
    .set(_nor.y * _nor.x - _nor.z * _nor.z, -_nor.z * _nor.y - _nor.x * _nor.x,
         _nor.x * _nor.z + _nor.y * _nor.y)
    .norm();

  _bin.set(_nor.y * _tan.z - _nor.z * _tan.y, _nor.z * _tan.x - _nor.x * _tan.z,
           _nor.x * _tan.y - _nor.y * _tan.x);

  // calculate hinge angle

  limite = _acosClamp(_an1.x * _an2.x + _an1.y * _an2.y + _an1.z * _an2.z);

  if (_nor.x * (_an1.y * _an2.z - _an1.z * _an2.y)
        + _nor.y * (_an1.z * _an2.x - _an1.x * _an2.z)
        + _nor.z * (_an1.x * _an2.y - _an1.y * _an2.x)
      < 0) {
    _limitMotor->angle = -limite;
  }
  else {
    _limitMotor->angle = limite;
  }

  tmp1X = _ax1.y * _ax2.z - _ax1.z * _ax2.y;
  tmp1Y = _ax1.z * _ax2.x - _ax1.x * _ax2.z;
  tmp1Z = _ax1.x * _ax2.y - _ax1.y * _ax2.x;

  _r3->limitMotor2->angle = _tan.x * tmp1X + _tan.y * tmp1Y + _tan.z * tmp1Z;
  _r3->limitMotor3->angle = _bin.x * tmp1X + _bin.y * tmp1Y + _bin.z * tmp1Z;

  _r3->preSolve(timeStep, invTimeStep);
  _lc->preSolve(timeStep, invTimeStep);
}

void HingeJoint::solve()
{
  _r3->solve();
  _lc->solve();
}

void HingeJoint::postSolve()
{
}

float HingeJoint::_acosClamp(float _cos) const
{
  if (_cos > 1) {
    return 0;
  }
  else if (_cos < -1.f) {
    return Math::PI;
  }
  else {
    return std::acos(_cos);
  }
}

} // end of namespace OIMO
