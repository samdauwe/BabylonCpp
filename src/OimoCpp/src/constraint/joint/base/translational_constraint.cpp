#include <oimo/constraint/joint/base/translational_constraint.h>

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

TranslationalConstraint::TranslationalConstraint(Joint* joint,
                                                 LimitMotor* limitMotor)
    : _limitState{0}
    , _enableMotor{false}
    , _limitMotor{limitMotor}
    , _limitImpulse{0.f}
    , _motorImpulse{0.f}
{
  _b1 = joint->body1;
  _b2 = joint->body2;
  _p1 = joint->anchorPoint1;
  _p2 = joint->anchorPoint2;
  _r1 = joint->relativeAnchorPoint1;
  _r2 = joint->relativeAnchorPoint2;
  _l1 = _b1->linearVelocity;
  _l2 = _b2->linearVelocity;
  _a1 = _b1->angularVelocity;
  _a2 = _b2->angularVelocity;
  _i1 = _b1->inverseInertia;
  _i2 = _b2->inverseInertia;
}

TranslationalConstraint::~TranslationalConstraint()
{
}

void TranslationalConstraint::preSolve(float timeStep, float invTimeStep)
{
  _ax            = _limitMotor->axis.x;
  _ay            = _limitMotor->axis.y;
  _az            = _limitMotor->axis.z;
  _lowerLimit    = _limitMotor->lowerLimit;
  _upperLimit    = _limitMotor->upperLimit;
  _motorSpeed    = _limitMotor->motorSpeed;
  _maxMotorForce = _limitMotor->maxMotorForce;
  _enableMotor   = _maxMotorForce > 0;
  _m1            = _b1->inverseMass;
  _m2            = _b2->inverseMass;

  const std::array<float, 9>& ti1 = _i1.elements;
  const std::array<float, 9>& ti2 = _i2.elements;
  _i1e00 = ti1[0];
  _i1e01 = ti1[1];
  _i1e02 = ti1[2];
  _i1e10 = ti1[3];
  _i1e11 = ti1[4];
  _i1e12 = ti1[5];
  _i1e20 = ti1[6];
  _i1e21 = ti1[7];
  _i1e22 = ti1[8];

  _i2e00 = ti2[0];
  _i2e01 = ti2[1];
  _i2e02 = ti2[2];
  _i2e10 = ti2[3];
  _i2e11 = ti2[4];
  _i2e12 = ti2[5];
  _i2e20 = ti2[6];
  _i2e21 = ti2[7];
  _i2e22 = ti2[8];

  float dx          = _p2.x - _p1.x;
  float dy          = _p2.y - _p1.y;
  float dz          = _p2.z - _p1.z;
  float d           = dx * _ax + dy * _ay + dz * _az;
  float frequency   = _limitMotor->frequency;
  bool enableSpring = frequency > 0;
  bool enableLimit  = _lowerLimit <= _upperLimit;
  if ((enableSpring && d > 20) || d < -20) {
    enableSpring = false;
  }

  if (enableLimit) {
    if (floats_are_equal(_lowerLimit, _upperLimit)) {
      if (_limitState != 0) {
        _limitState   = 0;
        _limitImpulse = 0.f;
      }
      _limitVelocity = _lowerLimit - d;
      if (!enableSpring) {
        d = _lowerLimit;
      }
    }
    else if (d < _lowerLimit) {
      if (_limitState != -1) {
        _limitState   = -1;
        _limitImpulse = 0.f;
      }
      _limitVelocity = _lowerLimit - d;
      if (!enableSpring) {
        d = _lowerLimit;
      }
    }
    else if (d > _upperLimit) {
      if (_limitState != 1) {
        _limitState   = 1;
        _limitImpulse = 0.f;
      }
      _limitVelocity = _upperLimit - d;
      if (!enableSpring) {
        d = _upperLimit;
      }
    }
    else {
      _limitState    = 2;
      _limitImpulse  = 0.f;
      _limitVelocity = 0.f;
    }
    if (!enableSpring) {
      if (_limitVelocity > 0.005f) {
        _limitVelocity -= 0.005f;
      }
      else if (_limitVelocity < -0.005f) {
        _limitVelocity += 0.005f;
      }
      else {
        _limitVelocity = 0.f;
      }
    }
  }
  else {
    _limitState   = 2;
    _limitImpulse = 0.f;
  }

  if (_enableMotor && (_limitState != 0 || enableSpring)) {
    _maxMotorImpulse = _maxMotorForce * timeStep;
  }
  else {
    _motorImpulse    = 0.f;
    _maxMotorImpulse = 0.f;
  }

  float rdx = d * _ax;
  float rdy = d * _ay;
  float rdz = d * _az;
  float w1  = _m1 / (_m1 + _m2);
  float w2  = 1.f - w1;
  _r1x      = _r1.x + rdx * w1;
  _r1y      = _r1.y + rdy * w1;
  _r1z      = _r1.z + rdz * w1;
  _r2x      = _r2.x - rdx * w2;
  _r2y      = _r2.y - rdy * w2;
  _r2z      = _r2.z - rdz * w2;

  _t1x        = _r1y * _az - _r1z * _ay;
  _t1y        = _r1z * _ax - _r1x * _az;
  _t1z        = _r1x * _ay - _r1y * _ax;
  _t2x        = _r2y * _az - _r2z * _ay;
  _t2y        = _r2z * _ax - _r2x * _az;
  _t2z        = _r2x * _ay - _r2y * _ax;
  _l1x        = _ax * _m1;
  _l1y        = _ay * _m1;
  _l1z        = _az * _m1;
  _l2x        = _ax * _m2;
  _l2y        = _ay * _m2;
  _l2z        = _az * _m2;
  _a1x        = _t1x * _i1e00 + _t1y * _i1e01 + _t1z * _i1e02;
  _a1y        = _t1x * _i1e10 + _t1y * _i1e11 + _t1z * _i1e12;
  _a1z        = _t1x * _i1e20 + _t1y * _i1e21 + _t1z * _i1e22;
  _a2x        = _t2x * _i2e00 + _t2y * _i2e01 + _t2z * _i2e02;
  _a2y        = _t2x * _i2e10 + _t2y * _i2e11 + _t2z * _i2e12;
  _a2z        = _t2x * _i2e20 + _t2y * _i2e21 + _t2z * _i2e22;
  _motorDenom = _m1 + _m2
                + _ax * (_a1y * _r1z - _a1z * _r1y + _a2y * _r2z - _a2z * _r2y)
                + _ay * (_a1z * _r1x - _a1x * _r1z + _a2z * _r2x - _a2x * _r2z)
                + _az * (_a1x * _r1y - _a1y * _r1x + _a2x * _r2y - _a2y * _r2x);

  _invMotorDenom = 1.f / _motorDenom;

  if (enableSpring && _limitState != 2) {
    float omega = 6.2831853f * frequency;
    float k     = omega * omega * timeStep;
    float dmp   = invTimeStep / (k + 2.f * _limitMotor->dampingRatio * omega);
    _cfm        = _motorDenom * dmp;
    _limitVelocity *= k * dmp;
  }
  else {
    _cfm = 0;
    _limitVelocity *= invTimeStep * 0.05f;
  }

  _invDenom = 1.f / (_motorDenom + _cfm);

  float totalImpulse = _limitImpulse + _motorImpulse;
  _l1.x += totalImpulse * _l1x;
  _l1.y += totalImpulse * _l1y;
  _l1.z += totalImpulse * _l1z;
  _a1.x += totalImpulse * _a1x;
  _a1.y += totalImpulse * _a1y;
  _a1.z += totalImpulse * _a1z;
  _l2.x -= totalImpulse * _l2x;
  _l2.y -= totalImpulse * _l2y;
  _l2.z -= totalImpulse * _l2z;
  _a2.x -= totalImpulse * _a2x;
  _a2.y -= totalImpulse * _a2y;
  _a2.z -= totalImpulse * _a2z;
}

void TranslationalConstraint::solve()
{
  float rvn = _ax * (_l2.x - _l1.x) + _ay * (_l2.y - _l1.y)
              + _az * (_l2.z - _l1.z) + _t2x * _a2.x - _t1x * _a1.x
              + _t2y * _a2.y - _t1y * _a1.y + _t2z * _a2.z - _t1z * _a1.z;

  // motor part
  float newMotorImpulse;
  if (_enableMotor) {
    newMotorImpulse       = (rvn - _motorSpeed) * _invMotorDenom;
    float oldMotorImpulse = _motorImpulse;
    _motorImpulse += newMotorImpulse;
    if (_motorImpulse > _maxMotorImpulse) {
      _motorImpulse = _maxMotorImpulse;
    }
    else if (_motorImpulse < -_maxMotorImpulse) {
      _motorImpulse = -_maxMotorImpulse;
    }
    newMotorImpulse = _motorImpulse - oldMotorImpulse;
    rvn -= newMotorImpulse * _motorDenom;
  }
  else {
    newMotorImpulse = 0.f;
  }

  // limit part
  float newLimitImpulse;
  if (_limitState != 2) {
    newLimitImpulse = (rvn - _limitVelocity - _limitImpulse * _cfm) * _invDenom;
    float oldLimitImpulse = _limitImpulse;
    _limitImpulse += newLimitImpulse;
    if (_limitImpulse * static_cast<float>(_limitState) < 0) {
      _limitImpulse = 0.f;
    }
    newLimitImpulse = _limitImpulse - oldLimitImpulse;
  }
  else {
    newLimitImpulse = 0.f;
  }

  float totalImpulse = newLimitImpulse + newMotorImpulse;
  _l1.x += totalImpulse * _l1x;
  _l1.y += totalImpulse * _l1y;
  _l1.z += totalImpulse * _l1z;
  _a1.x += totalImpulse * _a1x;
  _a1.y += totalImpulse * _a1y;
  _a1.z += totalImpulse * _a1z;
  _l2.x -= totalImpulse * _l2x;
  _l2.y -= totalImpulse * _l2y;
  _l2.z -= totalImpulse * _l2z;
  _a2.x -= totalImpulse * _a2x;
  _a2.y -= totalImpulse * _a2y;
  _a2.z -= totalImpulse * _a2z;
}

} // end of namespace OIMO
