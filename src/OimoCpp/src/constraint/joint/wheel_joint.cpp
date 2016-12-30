#include <oimo/constraint/joint/wheel_joint.h>

#include <cmath>

#include <oimo/constraint/joint/base/rotational_3_constraint.h>
#include <oimo/constraint/joint/base/translational_3_constraint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/mat33.h>
#include <oimo/math/quat.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

WheelJoint::WheelJoint(const JointConfig& config)
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
  type = Joint::Type::JOINT_WHEEL;

  float dot = _localAxis1X * _localAxis2X + _localAxis1Y * _localAxis2Y
              + _localAxis1Z * _localAxis2Z;
  float len = 0.f;
  if (dot > -1.f && dot < 1.f) {
    _localAngAxis1X = _localAxis2X - dot * _localAxis1X;
    _localAngAxis1Y = _localAxis2Y - dot * _localAxis1Y;
    _localAngAxis1Z = _localAxis2Z - dot * _localAxis1Z;
    _localAngAxis2X = _localAxis1X - dot * _localAxis2X;
    _localAngAxis2Y = _localAxis1Y - dot * _localAxis2Y;
    _localAngAxis2Z = _localAxis1Z - dot * _localAxis2Z;
    len             = 1.f / std::sqrt(_localAngAxis1X * _localAngAxis1X
                          + _localAngAxis1Y * _localAngAxis1Y
                          + _localAngAxis1Z * _localAngAxis1Z);
    _localAngAxis1X *= len;
    _localAngAxis1Y *= len;
    _localAngAxis1Z *= len;
    len = 1.f / std::sqrt(_localAngAxis2X * _localAngAxis2X
                          + _localAngAxis2Y * _localAngAxis2Y
                          + _localAngAxis2Z * _localAngAxis2Z);
    _localAngAxis2X *= len;
    _localAngAxis2Y *= len;
    _localAngAxis2Z *= len;
  }
  else {
    _localAngAxis1X = _localAxis1Y * _localAxis1X - _localAxis1Z * _localAxis1Z;
    _localAngAxis1Y
      = -_localAxis1Z * _localAxis1Y - _localAxis1X * _localAxis1X;
    _localAngAxis1Z = _localAxis1X * _localAxis1Z + _localAxis1Y * _localAxis1Y;
    len             = 1.f / std::sqrt(_localAngAxis1X * _localAngAxis1X
                          + _localAngAxis1Y * _localAngAxis1Y
                          + _localAngAxis1Z * _localAngAxis1Z);
    _localAngAxis1X *= len;
    _localAngAxis1Y *= len;
    _localAngAxis1Z *= len;
    Mat33 arc = Mat33().setQuat(Quat().arc(_localAxis1, _localAxis2));
    const std::array<float, 9>& tarc = arc.elements;
    _localAngAxis2X = _localAngAxis1X * tarc[0] + _localAngAxis1Y * tarc[1]
                      + _localAngAxis1Z * tarc[2];
    _localAngAxis2Y = _localAngAxis1X * tarc[3] + _localAngAxis1Y * tarc[4]
                      + _localAngAxis1Z * tarc[5];
    _localAngAxis2Z = _localAngAxis1X * tarc[6] + _localAngAxis1Y * tarc[7]
                      + _localAngAxis1Z * tarc[8];
  }

  // The translational limit and motor information of the joint.
  _translationalLimitMotor               = new LimitMotor(_tan, true);
  _translationalLimitMotor->frequency    = 8.f;
  _translationalLimitMotor->dampingRatio = 1.f;
  // The first rotational limit and motor information of the joint.
  _rotationalLimitMotor1 = new LimitMotor(_tan, false);
  // The second rotational limit and motor information of the joint.
  _rotationalLimitMotor2 = new LimitMotor(_bin, false);

  _t3 = new Translational3Constraint(this, new LimitMotor(_nor, true),
                                     _translationalLimitMotor,
                                     new LimitMotor(_bin, true));
  _t3->weight = 1.f;
  _r3
    = new Rotational3Constraint(this, new LimitMotor(_nor, true),
                                _rotationalLimitMotor1, _rotationalLimitMotor2);
}

WheelJoint::~WheelJoint()
{
}

void WheelJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();
  const std::array<float, 9>& tmp1M = body1->rotation.elements;
  float x1 = _localAxis1X * tmp1M[0] + _localAxis1Y * tmp1M[1]
             + _localAxis1Z * tmp1M[2];
  float y1 = _localAxis1X * tmp1M[3] + _localAxis1Y * tmp1M[4]
             + _localAxis1Z * tmp1M[5];
  float z1 = _localAxis1X * tmp1M[6] + _localAxis1Y * tmp1M[7]
             + _localAxis1Z * tmp1M[8];
  float angAxis1X = _localAngAxis1X * tmp1M[0] + _localAngAxis1Y * tmp1M[1]
                    + _localAngAxis1Z * tmp1M[2];
  float angAxis1Y = _localAngAxis1X * tmp1M[3] + _localAngAxis1Y * tmp1M[4]
                    + _localAngAxis1Z * tmp1M[5];
  float angAxis1Z = _localAngAxis1X * tmp1M[6] + _localAngAxis1Y * tmp1M[7]
                    + _localAngAxis1Z * tmp1M[8];
  const std::array<float, 9>& tmp2M = body2->rotation.elements;
  float x2 = _localAxis2X * tmp2M[0] + _localAxis2Y * tmp2M[1]
             + _localAxis2Z * tmp2M[2];
  float y2 = _localAxis2X * tmp2M[3] + _localAxis2Y * tmp2M[4]
             + _localAxis2Z * tmp2M[5];
  float z2 = _localAxis2X * tmp2M[6] + _localAxis2Y * tmp2M[7]
             + _localAxis2Z * tmp2M[8];
  float angAxis2X = _localAngAxis2X * tmp2M[0] + _localAngAxis2Y * tmp2M[1]
                    + _localAngAxis2Z * tmp2M[2];
  float angAxis2Y = _localAngAxis2X * tmp2M[3] + _localAngAxis2Y * tmp2M[4]
                    + _localAngAxis2Z * tmp2M[5];
  float angAxis2Z = _localAngAxis2X * tmp2M[6] + _localAngAxis2Y * tmp2M[7]
                    + _localAngAxis2Z * tmp2M[8];

  _r3->limitMotor1->angle = x1 * x2 + y1 * y2 + z1 * z2;
  if (x1 * (angAxis1Y * z2 - angAxis1Z * y2)
        + y1 * (angAxis1Z * x2 - angAxis1X * z2)
        + z1 * (angAxis1X * y2 - angAxis1Y * x2)
      < 0) {
    _rotationalLimitMotor1->angle
      = -_acosClamp(angAxis1X * x2 + angAxis1Y * y2 + angAxis1Z * z2);
  }
  else {
    _rotationalLimitMotor1->angle
      = _acosClamp(angAxis1X * x2 + angAxis1Y * y2 + angAxis1Z * z2);
  }
  if (x2 * (angAxis2Y * z1 - angAxis2Z * y1)
        + y2 * (angAxis2Z * x1 - angAxis2X * z1)
        + z2 * (angAxis2X * y1 - angAxis2Y * x1)
      < 0) {
    _rotationalLimitMotor2->angle
      = _acosClamp(angAxis2X * x1 + angAxis2Y * y1 + angAxis2Z * z1);
  }
  else {
    _rotationalLimitMotor2->angle
      = -_acosClamp(angAxis2X * x1 + angAxis2Y * y1 + angAxis2Z * z1);
  }

  float nx = y2 * z1 - z2 * y1;
  float ny = z2 * x1 - x2 * z1;
  float nz = x2 * y1 - y2 * x1;

  float tmp1X = std::sqrt(nx * nx + ny * ny + nz * nz);
  if (tmp1X > 0.f) {
    tmp1X = 1.f / tmp1X;
  }
  nx *= tmp1X;
  ny *= tmp1X;
  nz *= tmp1X;

  float tx = ny * z2 - nz * y2;
  float ty = nz * x2 - nx * z2;
  float tz = nx * y2 - ny * x2;

  tmp1X = std::sqrt(tx * tx + ty * ty + tz * tz);
  if (tmp1X > 0.f) {
    tmp1X = 1.f / tmp1X;
  }
  tx *= tmp1X;
  ty *= tmp1X;
  tz *= tmp1X;

  float bx = y1 * nz - z1 * ny;
  float by = z1 * nx - x1 * nz;
  float bz = x1 * ny - y1 * nx;

  tmp1X = std::sqrt(bx * bx + by * by + bz * bz);
  if (tmp1X > 0.f) {
    tmp1X = 1.f / tmp1X;
  }
  bx *= tmp1X;
  by *= tmp1X;
  bz *= tmp1X;

  _nor.init(nx, ny, nz);
  _tan.init(tx, ty, tz);
  _bin.init(bx, by, bz);

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

float WheelJoint::_acosClamp(float _cos) const
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
