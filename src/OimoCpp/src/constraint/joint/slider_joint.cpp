#include <oimo/constraint/joint/slider_joint.h>

#include <cmath>

#include <oimo/constraint/joint/base/rotational_3_constraint.h>
#include <oimo/constraint/joint/base/translational_3_constraint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/mat33.h>
#include <oimo/math/quat.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

SliderJoint::SliderJoint(const JointConfig& config, float lowerTranslation,
                         float upperTranslation)
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
  type = Joint::Type::JOINT_SLIDER;

  _localAngAxis1X = _localAxis1Y * _localAxis1X - _localAxis1Z * _localAxis1Z;
  _localAngAxis1Y = -_localAxis1Z * _localAxis1Y - _localAxis1X * _localAxis1X;
  _localAngAxis1Z = _localAxis1X * _localAxis1Z + _localAxis1Y * _localAxis1Y;

  float len = 1.f / std::sqrt(_localAngAxis1X * _localAngAxis1X
                              + _localAngAxis1Y * _localAngAxis1Y
                              + _localAngAxis1Z * _localAngAxis1Z);
  _localAngAxis1X *= len;
  _localAngAxis1Y *= len;
  _localAngAxis1Z *= len;

  // make angle axis 2
  Mat33 arc = Mat33().setQuat(Quat().arc(_localAxis1, _localAxis2));
  const std::array<float, 9>& tarc = arc.elements;
  _localAngAxis2X = _localAngAxis1X * tarc[0] + _localAngAxis1Y * tarc[1]
                    + _localAngAxis1Z * tarc[2];
  _localAngAxis2Y = _localAngAxis1X * tarc[3] + _localAngAxis1Y * tarc[4]
                    + _localAngAxis1Z * tarc[5];
  _localAngAxis2Z = _localAngAxis1X * tarc[6] + _localAngAxis1Y * tarc[7]
                    + _localAngAxis1Z * tarc[8];

  // The limit and motor for the rotation
  _rotationalLimitMotor = new LimitMotor(_nor, false);
  _r3                   = new Rotational3Constraint(this, _rotationalLimitMotor,
                                  new LimitMotor(_tan, true),
                                  new LimitMotor(_bin, true));
  // The limit and motor for the translation.
  _translationalLimitMotor             = new LimitMotor(_nor, true);
  _translationalLimitMotor->lowerLimit = lowerTranslation;
  _translationalLimitMotor->upperLimit = upperTranslation;
  _t3 = new Translational3Constraint(this, _translationalLimitMotor,
                                     new LimitMotor(_tan, true),
                                     new LimitMotor(_bin, true));
}

SliderJoint::~SliderJoint()
{
}

void SliderJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();

  const std::array<float, 9>& tmp1M = body1->rotation.elements;
  float axis1X = _localAxis1X * tmp1M[0] + _localAxis1Y * tmp1M[1]
                 + _localAxis1Z * tmp1M[2];
  float axis1Y = _localAxis1X * tmp1M[3] + _localAxis1Y * tmp1M[4]
                 + _localAxis1Z * tmp1M[5];
  float axis1Z = _localAxis1X * tmp1M[6] + _localAxis1Y * tmp1M[7]
                 + _localAxis1Z * tmp1M[8];
  float angAxis1X = _localAngAxis1X * tmp1M[0] + _localAngAxis1Y * tmp1M[1]
                    + _localAngAxis1Z * tmp1M[2];
  float angAxis1Y = _localAngAxis1X * tmp1M[3] + _localAngAxis1Y * tmp1M[4]
                    + _localAngAxis1Z * tmp1M[5];
  float angAxis1Z = _localAngAxis1X * tmp1M[6] + _localAngAxis1Y * tmp1M[7]
                    + _localAngAxis1Z * tmp1M[8];
  const std::array<float, 9>& tmp2M = body2->rotation.elements;
  float axis2X = _localAxis2X * tmp2M[0] + _localAxis2Y * tmp2M[1]
                 + _localAxis2Z * tmp2M[2];
  float axis2Y = _localAxis2X * tmp2M[3] + _localAxis2Y * tmp2M[4]
                 + _localAxis2Z * tmp2M[5];
  float axis2Z = _localAxis2X * tmp2M[6] + _localAxis2Y * tmp2M[7]
                 + _localAxis2Z * tmp2M[8];
  float angAxis2X = _localAngAxis2X * tmp2M[0] + _localAngAxis2Y * tmp2M[1]
                    + _localAngAxis2Z * tmp2M[2];
  float angAxis2Y = _localAngAxis2X * tmp2M[3] + _localAngAxis2Y * tmp2M[4]
                    + _localAngAxis2Z * tmp2M[5];
  float angAxis2Z = _localAngAxis2X * tmp2M[6] + _localAngAxis2Y * tmp2M[7]
                    + _localAngAxis2Z * tmp2M[8];

  float nx    = axis1X * body2->inverseMass + axis2X * body1->inverseMass;
  float ny    = axis1Y * body2->inverseMass + axis2Y * body1->inverseMass;
  float nz    = axis1Z * body2->inverseMass + axis2Z * body1->inverseMass;
  float tmp1X = std::sqrt(nx * nx + ny * ny + nz * nz);
  if (tmp1X > 0.f) {
    tmp1X = 1.f / tmp1X;
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

  // ----------------------------------------------
  //            calculate hinge angle
  // ----------------------------------------------

  if (nx * (angAxis1Y * angAxis2Z - angAxis1Z * angAxis2Y)
        + ny * (angAxis1Z * angAxis2X - angAxis1X * angAxis2Z)
        + nz * (angAxis1X * angAxis2Y - angAxis1Y * angAxis2X)
      < 0) {
    _rotationalLimitMotor->angle = -_acosClamp(
      angAxis1X * angAxis2X + angAxis1Y * angAxis2Y + angAxis1Z * angAxis2Z);
  }
  else {
    _rotationalLimitMotor->angle = _acosClamp(
      angAxis1X * angAxis2X + angAxis1Y * angAxis2Y + angAxis1Z * angAxis2Z);
  }

  // angular error
  tmp1X       = axis1Y * axis2Z - axis1Z * axis2Y;
  float tmp1Y = axis1Z * axis2X - axis1X * axis2Z;
  float tmp1Z = axis1X * axis2Y - axis1Y * axis2X;

  _r3->limitMotor2->angle = tx * tmp1X + ty * tmp1Y + tz * tmp1Z;
  _r3->limitMotor3->angle = bx * tmp1X + by * tmp1Y + bz * tmp1Z;

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

float SliderJoint::_acosClamp(float _cos) const
{
  if (_cos > 1.f) {
    return 0.f;
  }
  else if (_cos < -1.f) {
    return Math::PI;
  }
  else {
    return std::acos(_cos);
  }
}

} // end of namespace OIMO
