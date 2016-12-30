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
    , _localAxis1{Vec3().normalize(config.localAxis1)}
    , _localAxis2{Vec3().normalize(config.localAxis2)}
    , _localAxis1X{_localAxis1.x}
    , _localAxis1Y{_localAxis1.y}
    , _localAxis1Z{_localAxis1.z}
    , _localAxis2X{_localAxis2.x}
    , _localAxis2Y{_localAxis2.y}
    , _localAxis2Z{_localAxis2.z}
{
  // Set joint type
  type = Joint::Type::JOINT_PRISMATIC;

  _ac = new AngularConstraint(this, Quat().arc(_localAxis1, _localAxis2));
  _limitMotor             = new LimitMotor(_nor, true);
  _limitMotor->lowerLimit = lowerTranslation;
  _limitMotor->upperLimit = upperTranslation;
  _t3                     = new Translational3Constraint(
    this, _limitMotor, new LimitMotor(_tan, true), new LimitMotor(_bin, true));
}

PrismaticJoint::~PrismaticJoint()
{
}

void PrismaticJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();

  const std::array<float, 9>& tmp1M = body1->rotation.elements;
  float axis1X = _localAxis1X * tmp1M[0] + _localAxis1Y * tmp1M[1]
                 + _localAxis1Z * tmp1M[2];
  float axis1Y = _localAxis1X * tmp1M[3] + _localAxis1Y * tmp1M[4]
                 + _localAxis1Z * tmp1M[5];
  float axis1Z = _localAxis1X * tmp1M[6] + _localAxis1Y * tmp1M[7]
                 + _localAxis1Z * tmp1M[8];
  const std::array<float, 9>& tmp2M = body2->rotation.elements;
  float axis2X = _localAxis2X * tmp2M[0] + _localAxis2Y * tmp2M[1]
                 + _localAxis2Z * tmp2M[2];
  float axis2Y = _localAxis2X * tmp2M[3] + _localAxis2Y * tmp2M[4]
                 + _localAxis2Z * tmp2M[5];
  float axis2Z = _localAxis2X * tmp2M[6] + _localAxis2Y * tmp2M[7]
                 + _localAxis2Z * tmp2M[8];

  float nx    = axis1X * body2->inverseMass + axis2X * body1->inverseMass;
  float ny    = axis1Y * body2->inverseMass + axis2Y * body1->inverseMass;
  float nz    = axis1Z * body2->inverseMass + axis2Z * body1->inverseMass;
  float tmp1X = std::sqrt(nx * nx + ny * ny + nz * nz);
  if (tmp1X > 0.f) {
    tmp1X = 1 / tmp1X;
  }
  nx *= tmp1X;
  ny *= tmp1X;
  nz *= tmp1X;
  float tx = ny * nx - nz * nz;
  float ty = -nz * ny - nx * nx;
  float tz = nx * nz + ny * ny;
  tmp1X    = 1.f / std::sqrt(tx * tx + ty * ty + tz * tz);
  tx *= tmp1X;
  ty *= tmp1X;
  tz *= tmp1X;
  float bx = ny * tz - nz * ty;
  float by = nz * tx - nx * tz;
  float bz = nx * ty - ny * tx;

  _nor.init(nx, ny, nz);
  _tan.init(tx, ty, tz);
  _bin.init(bx, by, bz);

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
