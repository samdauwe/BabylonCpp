#include <oimo/constraint/joint/prismatic_joint.h>

#include <cmath>

#include <oimo/constraint/joint/base/angular_constraint.h>
#include <oimo/constraint/joint/base/translational_3_constraint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/quat.h>

namespace OIMO {

PrismaticJoint::PrismaticJoint(const JointConfig& config,
                               float lowerTranslation, float upperTranslation)
    : Joint{config}
    , _limitMotorTan{make_unique<LimitMotor>(_tan, true)}
    , _limitMotorBin{make_unique<LimitMotor>(_bin, true)}
{
  // Set joint type
  type = Joint::Type::JOINT_PRISMATIC;

  // The axis in the first body's coordinate system.
  _localAxis1 = config.localAxis1.clone().normalize();
  // The axis in the second body's coordinate system.
  _localAxis2 = config.localAxis2.clone().normalize();

  _ac = make_unique<AngularConstraint>(
    this, Quat().setFromUnitVectors(_localAxis1, _localAxis2));
  _limitMotor             = make_unique<LimitMotor>(_nor, true);
  _limitMotor->lowerLimit = lowerTranslation;
  _limitMotor->upperLimit = upperTranslation;
  _t3                     = make_unique<Translational3Constraint>(
    this, _limitMotor.get(), _limitMotorTan.get(), _limitMotorBin.get());
}

PrismaticJoint::~PrismaticJoint()
{
}

void PrismaticJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();

  _ax1.copy(_localAxis1).applyMatrix3(body1->rotation, true);
  _ax2.copy(_localAxis2).applyMatrix3(body2->rotation, true);

  // normal tangent binormal

  _nor
    .set(_ax1.x * body2->inverseMass + _ax2.x * body1->inverseMass,
         _ax1.y * body2->inverseMass + _ax2.y * body1->inverseMass,
         _ax1.z * body2->inverseMass + _ax2.z * body1->inverseMass)
    .normalize();
  _tan.tangent(_nor).normalize();
  _bin.crossVectors(_nor, _tan);

  // preSolve

  _ac->preSolve(timeStep, invTimeStep);
  _t3->preSolve(timeStep, invTimeStep);
}

void PrismaticJoint::solve()
{
  _ac->solve();
  _t3->solve();
}

void PrismaticJoint::postSolve()
{
}

} // end of namespace OIMO
