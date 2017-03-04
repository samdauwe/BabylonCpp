#include <oimo/constraint/joint/wheel_joint.h>

#include <cmath>

#include <oimo/constraint/joint/base/rotational_3_constraint.h>
#include <oimo/constraint/joint/base/translational_3_constraint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/mat33.h>
#include <oimo/math/math_utils.h>
#include <oimo/math/quat.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

WheelJoint::WheelJoint(const JointConfig& config)
    : Joint{config}
    , _translationalLimitMotorNor{make_unique<LimitMotor>(_nor, true)}
    , _translationalLimitMotorBin{make_unique<LimitMotor>(_bin, true)}
    , _rotationalLimitMotorNor{make_unique<LimitMotor>(_nor, true)}
{
  // Set joint type
  type = Joint::Type::JOINT_WHEEL;

  // The axis in the first body's coordinate system.
  _localAxis1 = config.localAxis1.clone().normalize();
  // The axis in the second body's coordinate system.
  _localAxis2 = config.localAxis2.clone().normalize();

  float dot = Math::dotVectors(_localAxis1, _localAxis2);

  if (dot > -1.f && dot < 1.f) {
    _localAngle1
      .set(_localAxis2.x - dot * _localAxis1.x,
           _localAxis2.y - dot * _localAxis1.y,
           _localAxis2.z - dot * _localAxis1.z)
      .normalize();

    _localAngle2
      .set(_localAxis1.x - dot * _localAxis2.x,
           _localAxis1.y - dot * _localAxis2.y,
           _localAxis1.z - dot * _localAxis2.z)
      .normalize();
  }
  else {
    Mat33 arc
      = Mat33().setQuat(Quat().setFromUnitVectors(_localAxis1, _localAxis2));
    _localAngle1.tangent(_localAxis1).normalize();
    _localAngle2 = _localAngle1.clone().applyMatrix3(arc, true);
  }

  // The translational limit and motor information of the joint.
  _translationalLimitMotor               = make_unique<LimitMotor>(_tan, true);
  _translationalLimitMotor->frequency    = 8;
  _translationalLimitMotor->dampingRatio = 1;
  // The first rotational limit and motor information of the joint.
  _rotationalLimitMotor1 = make_unique<LimitMotor>(_tan, false);
  // The second rotational limit and motor information of the joint.
  _rotationalLimitMotor2 = make_unique<LimitMotor>(_bin, false);

  _t3 = make_unique<Translational3Constraint>(
    this, _translationalLimitMotorNor.get(), _translationalLimitMotor.get(),
    _translationalLimitMotorBin.get());
  _t3->weight = 1;
  _r3 = make_unique<Rotational3Constraint>(this, _rotationalLimitMotorNor.get(),
                                           _rotationalLimitMotor1.get(),
                                           _rotationalLimitMotor2.get());
}

WheelJoint::~WheelJoint()
{
}

void WheelJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();

  _ax1.copy(_localAxis1).applyMatrix3(body1->rotation, true);
  _an1.copy(_localAngle1).applyMatrix3(body1->rotation, true);

  _ax2.copy(_localAxis2).applyMatrix3(body2->rotation, true);
  _an2.copy(_localAngle2).applyMatrix3(body2->rotation, true);

  _r3->limitMotor1->angle = Math::dotVectors(_ax1, _ax2);

  float limite = Math::dotVectors(_an1, _ax2);

  if (Math::dotVectors(_ax1, _tmp.crossVectors(_an1, _ax2)) < 0.f) {
    _rotationalLimitMotor1->angle = -limite;
  }
  else {
    _rotationalLimitMotor1->angle = limite;
  }

  limite = Math::dotVectors(_an2, _ax1);

  if (Math::dotVectors(_ax2, _tmp.crossVectors(_an2, _ax1)) < 0.f) {
    _rotationalLimitMotor2->angle = -limite;
  }
  else {
    _rotationalLimitMotor2->angle = limite;
  }

  _nor.crossVectors(_ax1, _ax2).normalize();
  _tan.crossVectors(_nor, _ax2).normalize();
  _bin.crossVectors(_nor, _ax1).normalize();

  _r3->preSolve(timeStep, invTimeStep);
  _t3->preSolve(timeStep, invTimeStep);
}
void WheelJoint::solve()
{
  _r3->solve();
  _t3->solve();
}
void WheelJoint::postSolve()
{
}

} // end of namespace OIMO
