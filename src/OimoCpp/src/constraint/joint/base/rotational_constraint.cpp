#include <oimo/constraint/joint/base/rotational_constraint.h>

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

RotationalConstraint::RotationalConstraint(Joint* joint, LimitMotor* limitMotor)
    : _limitState{0}
    , _enableMotor{false}
    , _limitMotor{limitMotor}
    , _limitImpulse{0.f}
    , _motorImpulse{0.f}
{
  _b1 = joint->body1;
  _b2 = joint->body2;
  _a1 = _b1->angularVelocity;
  _a2 = _b2->angularVelocity;
  _i1 = _b1->inverseInertia;
  _i2 = _b2->inverseInertia;
}

RotationalConstraint::~RotationalConstraint()
{
}

void RotationalConstraint::preSolve(float timeStep, float invTimeStep)
{
  _ax            = _limitMotor->axis.x;
  _ay            = _limitMotor->axis.y;
  _az            = _limitMotor->axis.z;
  _lowerLimit    = _limitMotor->lowerLimit;
  _upperLimit    = _limitMotor->upperLimit;
  _motorSpeed    = _limitMotor->motorSpeed;
  _maxMotorForce = _limitMotor->maxMotorForce;
  _enableMotor   = _maxMotorForce > 0;

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

  float frequency   = _limitMotor->frequency;
  bool enableSpring = frequency > 0;
  bool enableLimit  = _lowerLimit <= _upperLimit;
  float angle       = _limitMotor->angle;
  if (enableLimit) {
    if (floats_are_equal(_lowerLimit, _upperLimit)) {
      if (_limitState != 0) {
        _limitState   = 0;
        _limitImpulse = 0.f;
      }
      _limitVelocity = _lowerLimit - angle;
    }
    else if (angle < _lowerLimit) {
      if (_limitState != -1) {
        _limitState   = -1;
        _limitImpulse = 0.f;
      }
      _limitVelocity = _lowerLimit - angle;
    }
    else if (angle > _upperLimit) {
      if (_limitState != 1) {
        _limitState   = 1;
        _limitImpulse = 0.f;
      }
      _limitVelocity = _upperLimit - angle;
    }
    else {
      _limitState    = 2;
      _limitImpulse  = 0.f;
      _limitVelocity = 0.f;
    }
    if (!enableSpring) {
      if (_limitVelocity > 0.02f)
        _limitVelocity -= 0.02f;
      else if (_limitVelocity < -0.02f)
        _limitVelocity += 0.02f;
      else
        _limitVelocity = 0.f;
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

  _a1x        = _ax * _i1e00 + _ay * _i1e01 + _az * _i1e02;
  _a1y        = _ax * _i1e10 + _ay * _i1e11 + _az * _i1e12;
  _a1z        = _ax * _i1e20 + _ay * _i1e21 + _az * _i1e22;
  _a2x        = _ax * _i2e00 + _ay * _i2e01 + _az * _i2e02;
  _a2y        = _ax * _i2e10 + _ay * _i2e11 + _az * _i2e12;
  _a2z        = _ax * _i2e20 + _ay * _i2e21 + _az * _i2e22;
  _motorDenom = _ax * (_a1x + _a2x) + _ay * (_a1y + _a2y) + _az * (_a1z + _a2z);
  _invMotorDenom = 1.f / _motorDenom;

  if (enableSpring && _limitState != 2) {
    float omega = 6.2831853f * frequency;
    float k     = omega * omega * timeStep;
    float dmp   = invTimeStep / (k + 2.f * _limitMotor->dampingRatio * omega);
    _cfm        = _motorDenom * dmp;
    _limitVelocity *= k * dmp;
  }
  else {
    _cfm = 0.f;
    _limitVelocity *= invTimeStep * 0.05f;
  }

  _invDenom = 1.f / (_motorDenom + _cfm);

  _limitImpulse *= 0.95f;
  _motorImpulse *= 0.95f;
  float totalImpulse = _limitImpulse + _motorImpulse;
  _a1.x += totalImpulse * _a1x;
  _a1.y += totalImpulse * _a1y;
  _a1.z += totalImpulse * _a1z;
  _a2.x -= totalImpulse * _a2x;
  _a2.y -= totalImpulse * _a2y;
  _a2.z -= totalImpulse * _a2z;
}

void RotationalConstraint::solve()
{
  float rvn
    = _ax * (_a2.x - _a1.x) + _ay * (_a2.y - _a1.y) + _az * (_a2.z - _a1.z);

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
  _a1.x += totalImpulse * _a1x;
  _a1.y += totalImpulse * _a1y;
  _a1.z += totalImpulse * _a1z;
  _a2.x -= totalImpulse * _a2x;
  _a2.y -= totalImpulse * _a2y;
  _a2.z -= totalImpulse * _a2z;
}

} // end of namespace OIMO
