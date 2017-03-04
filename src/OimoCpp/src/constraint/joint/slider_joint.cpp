#include <oimo/constraint/joint/slider_joint.h>

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

SliderJoint::SliderJoint(const JointConfig& config, float lowerTranslation,
                         float upperTranslation)
    : Joint{config}
    , _rotationalLimitMotor{make_unique<LimitMotor>(_nor, false)}
    , _rotationalLimitMotorTan{make_unique<LimitMotor>(_tan, true)}
    , _rotationalLimitMotorBin{make_unique<LimitMotor>(_bin, true)}
    , _translationalLimitMotor{make_unique<LimitMotor>(_nor, true)}
    , _translationalLimitMotorTan{make_unique<LimitMotor>(_tan, true)}
    , _translationalLimitMotorBin{make_unique<LimitMotor>(_bin, true)}
{
  // Set joint type
  type = Joint::Type::JOINT_SLIDER;

  // The axis in the first body's coordinate system.
  _localAxis1 = config.localAxis1.clone().normalize();
  // The axis in the second body's coordinate system.
  _localAxis2 = config.localAxis2.clone().normalize();

  // make angle axis
  Mat33 arc
    = Mat33().setQuat(Quat().setFromUnitVectors(_localAxis1, _localAxis2));
  _localAngle1 = Vec3().tangent(_localAxis1).normalize();
  _localAngle2 = _localAngle1.clone().applyMatrix3(arc, true);

  // The limit and motor for the rotation
  _r3 = make_unique<Rotational3Constraint>(this, _rotationalLimitMotor.get(),
                                           _rotationalLimitMotorTan.get(),
                                           _rotationalLimitMotorBin.get());

  // The limit and motor for the translation.
  _translationalLimitMotor->lowerLimit = lowerTranslation;
  _translationalLimitMotor->upperLimit = upperTranslation;
  _t3                                  = make_unique<Translational3Constraint>(
    this, _translationalLimitMotor.get(), _translationalLimitMotorTan.get(),
    _translationalLimitMotorBin.get());
}

SliderJoint::~SliderJoint()
{
}

void SliderJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();

  _ax1.copy(_localAxis1).applyMatrix3(body1->rotation, true);
  _an1.copy(_localAngle1).applyMatrix3(body1->rotation, true);

  _ax2.copy(_localAxis2).applyMatrix3(body2->rotation, true);
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

  _tmp.crossVectors(_an1, _an2);

  float limite = Math::acosClamp(Math::dotVectors(_an1, _an2));

  if (Math::dotVectors(_nor, _tmp) < 0.f) {
    _rotationalLimitMotor->angle = -limite;
  }
  else {
    _rotationalLimitMotor->angle = limite;
  }

  // angular error

  _tmp.crossVectors(_ax1, _ax2);
  _r3->limitMotor2->angle = Math::dotVectors(_tan, _tmp);
  _r3->limitMotor3->angle = Math::dotVectors(_bin, _tmp);

  // preSolve

  _r3->preSolve(timeStep, invTimeStep);
  _t3->preSolve(timeStep, invTimeStep);
}

void SliderJoint::solve()
{
  _r3->solve();
  _t3->solve();
}

void SliderJoint::postSolve()
{
}

} // end of namespace OIMO
