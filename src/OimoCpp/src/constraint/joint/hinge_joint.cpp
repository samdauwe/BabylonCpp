#include <oimo/constraint/joint/hinge_joint.h>

#include <cmath>

#include <oimo/constraint/joint/base/linear_constraint.h>
#include <oimo/constraint/joint/base/rotational_3_constraint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/math_utils.h>
#include <oimo/math/quat.h>

namespace OIMO {

HingeJoint::HingeJoint(const JointConfig& config, float lowerAngleLimit,
                       float upperAngleLimit)
    : Joint{config}
    , _limitMotorTan{make_unique<LimitMotor>(_tan, true)}
    , _limitMotorBin{make_unique<LimitMotor>(_bin, true)}
{
  // Set joint type
  type = Joint::Type::JOINT_HINGE;

  // The axis in the first body's coordinate system.
  _localAxis1 = config.localAxis1.clone().normalize();
  // The axis in the second body's coordinate system.
  _localAxis2 = config.localAxis2.clone().normalize();

  // make angle axis
  Mat33 arc
    = Mat33().setQuat(Quat().setFromUnitVectors(_localAxis1, _localAxis2));
  _localAngle1 = Vec3().tangent(_localAxis1).normalize();
  _localAngle2 = _localAngle1.clone().applyMatrix3(arc, true);

  // The rotational limit and motor information of the joint.
  _limitMotor             = make_unique<LimitMotor>(_nor, false);
  _limitMotor->lowerLimit = lowerAngleLimit;
  _limitMotor->upperLimit = upperAngleLimit;

  _lc = make_unique<LinearConstraint>(this);
  _r3 = make_unique<Rotational3Constraint>(
    this, _limitMotor.get(), _limitMotorTan.get(), _limitMotorBin.get());
}

HingeJoint::~HingeJoint()
{
}

void HingeJoint::preSolve(float timeStep, float invTimeStep)
{
  Vec3 tmp;
  float limite;

  updateAnchorPoints();

  _ax1.copy(_localAxis1).applyMatrix3(body1->rotation, true);
  _ax2.copy(_localAxis2).applyMatrix3(body2->rotation, true);

  _an1.copy(_localAngle1).applyMatrix3(body1->rotation, true);
  _an2.copy(_localAngle2).applyMatrix3(body2->rotation, true);

  // normal tangent binormal

  _nor
    .set(_ax1.x * body2->inverseMass + _ax2.x * body1->inverseMass,
         _ax1.y * body2->inverseMass + _ax2.y * body1->inverseMass,
         _ax1.z * body2->inverseMass + _ax2.z * body1->inverseMass)
    .normalize();

  _tan.tangent(_nor).normalize();

  _bin.crossVectors(_nor, _tan);

  // calculate hinge angle

  limite = Math::acosClamp(Math::dotVectors(_an1, _an2));

  tmp.crossVectors(_an1, _an2);

  if (Math::dotVectors(_nor, tmp) < 0.f) {
    _limitMotor->angle = -limite;
  }
  else {
    _limitMotor->angle = limite;
  }

  tmp.crossVectors(_ax1, _ax2);

  _r3->limitMotor2->angle = Math::dotVectors(_tan, tmp);
  _r3->limitMotor3->angle = Math::dotVectors(_bin, tmp);

  // preSolve

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

} // end of namespace OIMO
