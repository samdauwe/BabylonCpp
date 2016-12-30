#include <oimo/constraint/joint/base/translational_3_constraint.h>

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/limit_motor.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

Translational3Constraint::Translational3Constraint(Joint* joint,
                                                   LimitMotor* limitMotor1,
                                                   LimitMotor* limitMotor2,
                                                   LimitMotor* limitMotor3)
    : weight{-1.f}
    , _limitState1{0}
    , _enableMotor1{false}
    , _limitState2{0}
    , _enableMotor2{false}
    , _limitState3{0}
    , _enableMotor3{false}
    , _limitMotor1{limitMotor1}
    , _limitMotor2{limitMotor2}
    , _limitMotor3{limitMotor3}
    , _limitImpulse1{0.f}
    , _motorImpulse1{0.f}
    , _limitImpulse2{0.f}
    , _motorImpulse2{0.f}
    , _limitImpulse3{0.f}
    , _motorImpulse3{0.f}
    , _cfm1{0.f}
    , _cfm2{0.f}
    , _cfm3{0.f}
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

Translational3Constraint::~Translational3Constraint()
{
}

void Translational3Constraint::preSolve(float timeStep, float invTimeStep)
{
  _ax1            = _limitMotor1->axis.x;
  _ay1            = _limitMotor1->axis.y;
  _az1            = _limitMotor1->axis.z;
  _ax2            = _limitMotor2->axis.x;
  _ay2            = _limitMotor2->axis.y;
  _az2            = _limitMotor2->axis.z;
  _ax3            = _limitMotor3->axis.x;
  _ay3            = _limitMotor3->axis.y;
  _az3            = _limitMotor3->axis.z;
  _lowerLimit1    = _limitMotor1->lowerLimit;
  _upperLimit1    = _limitMotor1->upperLimit;
  _motorSpeed1    = _limitMotor1->motorSpeed;
  _maxMotorForce1 = _limitMotor1->maxMotorForce;
  _enableMotor1   = _maxMotorForce1 > 0;
  _lowerLimit2    = _limitMotor2->lowerLimit;
  _upperLimit2    = _limitMotor2->upperLimit;
  _motorSpeed2    = _limitMotor2->motorSpeed;
  _maxMotorForce2 = _limitMotor2->maxMotorForce;
  _enableMotor2   = _maxMotorForce2 > 0;
  _lowerLimit3    = _limitMotor3->lowerLimit;
  _upperLimit3    = _limitMotor3->upperLimit;
  _motorSpeed3    = _limitMotor3->motorSpeed;
  _maxMotorForce3 = _limitMotor3->maxMotorForce;
  _enableMotor3   = _maxMotorForce3 > 0;
  _m1             = _b1->inverseMass;
  _m2             = _b2->inverseMass;

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

  float dx           = _p2.x - _p1.x;
  float dy           = _p2.y - _p1.y;
  float dz           = _p2.z - _p1.z;
  float d1           = dx * _ax1 + dy * _ay1 + dz * _az1;
  float d2           = dx * _ax2 + dy * _ay2 + dz * _az2;
  float d3           = dx * _ax3 + dy * _ay3 + dz * _az3;
  float frequency1   = _limitMotor1->frequency;
  float frequency2   = _limitMotor2->frequency;
  float frequency3   = _limitMotor3->frequency;
  bool enableSpring1 = frequency1 > 0;
  bool enableSpring2 = frequency2 > 0;
  bool enableSpring3 = frequency3 > 0;
  bool enableLimit1  = _lowerLimit1 <= _upperLimit1;
  bool enableLimit2  = _lowerLimit2 <= _upperLimit2;
  bool enableLimit3  = _lowerLimit3 <= _upperLimit3;

  // for stability
  if ((enableSpring1 && d1 > 20) || d1 < -20) {
    enableSpring1 = false;
  }
  if ((enableSpring2 && d2 > 20) || d2 < -20) {
    enableSpring2 = false;
  }
  if ((enableSpring3 && d3 > 20) || d3 < -20) {
    enableSpring3 = false;
  }

  if (enableLimit1) {
    if (floats_are_equal(_lowerLimit1, _upperLimit1)) {
      if (_limitState1 != 0) {
        _limitState1   = 0;
        _limitImpulse1 = 0.f;
      }
      _limitVelocity1 = _lowerLimit1 - d1;
      if (!enableSpring1) {
        d1 = _lowerLimit1;
      }
    }
    else if (d1 < _lowerLimit1) {
      if (_limitState1 != -1) {
        _limitState1   = -1;
        _limitImpulse1 = 0.f;
      }
      _limitVelocity1 = _lowerLimit1 - d1;
      if (!enableSpring1) {
        d1 = _lowerLimit1;
      }
    }
    else if (d1 > _upperLimit1) {
      if (_limitState1 != 1) {
        _limitState1   = 1;
        _limitImpulse1 = 0.f;
      }
      _limitVelocity1 = _upperLimit1 - d1;
      if (!enableSpring1) {
        d1 = _upperLimit1;
      }
    }
    else {
      _limitState1    = 2;
      _limitImpulse1  = 0.f;
      _limitVelocity1 = 0.f;
    }
    if (!enableSpring1) {
      if (_limitVelocity1 > 0.005f) {
        _limitVelocity1 -= 0.005f;
      }
      else if (_limitVelocity1 < -0.005f) {
        _limitVelocity1 += 0.005f;
      }
      else {
        _limitVelocity1 = 0.f;
      }
    }
  }
  else {
    _limitState1   = 2;
    _limitImpulse1 = 0.f;
  }

  if (enableLimit2) {
    if (floats_are_equal(_lowerLimit2, _upperLimit2)) {
      if (_limitState2 != 0) {
        _limitState2   = 0;
        _limitImpulse2 = 0.f;
      }
      _limitVelocity2 = _lowerLimit2 - d2;
      if (!enableSpring2) {
        d2 = _lowerLimit2;
      }
    }
    else if (d2 < _lowerLimit2) {
      if (_limitState2 != -1) {
        _limitState2   = -1;
        _limitImpulse2 = 0.f;
      }
      _limitVelocity2 = _lowerLimit2 - d2;
      if (!enableSpring2) {
        d2 = _lowerLimit2;
      }
    }
    else if (d2 > _upperLimit2) {
      if (_limitState2 != 1) {
        _limitState2   = 1;
        _limitImpulse2 = 0.f;
      }
      _limitVelocity2 = _upperLimit2 - d2;
      if (!enableSpring2) {
        d2 = _upperLimit2;
      }
    }
    else {
      _limitState2    = 2;
      _limitImpulse2  = 0.f;
      _limitVelocity2 = 0.f;
    }
    if (!enableSpring2) {
      if (_limitVelocity2 > 0.005f) {
        _limitVelocity2 -= 0.005f;
      }
      else if (_limitVelocity2 < -0.005f) {
        _limitVelocity2 += 0.005f;
      }
      else {
        _limitVelocity2 = 0.f;
      }
    }
  }
  else {
    _limitState2   = 2;
    _limitImpulse2 = 0.f;
  }

  if (enableLimit3) {
    if (floats_are_equal(_lowerLimit3, _upperLimit3)) {
      if (_limitState3 != 0) {
        _limitState3   = 0;
        _limitImpulse3 = 0.f;
      }
      _limitVelocity3 = _lowerLimit3 - d3;
      if (!enableSpring3) {
        d3 = _lowerLimit3;
      }
    }
    else if (d3 < _lowerLimit3) {
      if (_limitState3 != -1) {
        _limitState3   = -1;
        _limitImpulse3 = 0.f;
      }
      _limitVelocity3 = _lowerLimit3 - d3;
      if (!enableSpring3) {
        d3 = _lowerLimit3;
      }
    }
    else if (d3 > _upperLimit3) {
      if (_limitState3 != 1) {
        _limitState3   = 1;
        _limitImpulse3 = 0.f;
      }
      _limitVelocity3 = _upperLimit3 - d3;
      if (!enableSpring3) {
        d3 = _upperLimit3;
      }
    }
    else {
      _limitState3    = 2;
      _limitImpulse3  = 0.f;
      _limitVelocity3 = 0.f;
    }
    if (!enableSpring3) {
      if (_limitVelocity3 > 0.005f) {
        _limitVelocity3 -= 0.005f;
      }
      else if (_limitVelocity3 < -0.005f) {
        _limitVelocity3 += 0.005f;
      }
      else {
        _limitVelocity3 = 0.f;
      }
    }
  }
  else {
    _limitState3   = 2;
    _limitImpulse3 = 0.f;
  }

  if (_enableMotor1 && (_limitState1 != 0 || enableSpring1)) {
    _maxMotorImpulse1 = _maxMotorForce1 * timeStep;
  }
  else {
    _motorImpulse1    = 0.f;
    _maxMotorImpulse1 = 0.f;
  }

  if (_enableMotor2 && (_limitState2 != 0 || enableSpring2)) {
    _maxMotorImpulse2 = _maxMotorForce2 * timeStep;
  }
  else {
    _motorImpulse2    = 0.f;
    _maxMotorImpulse2 = 0.f;
  }

  if (_enableMotor3 && (_limitState3 != 0 || enableSpring3)) {
    _maxMotorImpulse3 = _maxMotorForce3 * timeStep;
  }
  else {
    _motorImpulse3    = 0.f;
    _maxMotorImpulse3 = 0.f;
  }

  float rdx = d1 * _ax1 + d2 * _ax2 + d3 * _ax2;
  float rdy = d1 * _ay1 + d2 * _ay2 + d3 * _ay2;
  float rdz = d1 * _az1 + d2 * _az2 + d3 * _az2;
  float w1  = _m2 / (_m1 + _m2);
  if (weight >= 0.f) {
    w1 = weight; // use given weight
  }
  float w2 = 1 - w1;
  _r1x     = _r1.x + rdx * w1;
  _r1y     = _r1.y + rdy * w1;
  _r1z     = _r1.z + rdz * w1;
  _r2x     = _r2.x - rdx * w2;
  _r2y     = _r2.y - rdy * w2;
  _r2z     = _r2.z - rdz * w2;

  // build jacobians
  _t1x1 = _r1y * _az1 - _r1z * _ay1;
  _t1y1 = _r1z * _ax1 - _r1x * _az1;
  _t1z1 = _r1x * _ay1 - _r1y * _ax1;
  _t2x1 = _r2y * _az1 - _r2z * _ay1;
  _t2y1 = _r2z * _ax1 - _r2x * _az1;
  _t2z1 = _r2x * _ay1 - _r2y * _ax1;
  _l1x1 = _ax1 * _m1;
  _l1y1 = _ay1 * _m1;
  _l1z1 = _az1 * _m1;
  _l2x1 = _ax1 * _m2;
  _l2y1 = _ay1 * _m2;
  _l2z1 = _az1 * _m2;
  _a1x1 = _t1x1 * _i1e00 + _t1y1 * _i1e01 + _t1z1 * _i1e02;
  _a1y1 = _t1x1 * _i1e10 + _t1y1 * _i1e11 + _t1z1 * _i1e12;
  _a1z1 = _t1x1 * _i1e20 + _t1y1 * _i1e21 + _t1z1 * _i1e22;
  _a2x1 = _t2x1 * _i2e00 + _t2y1 * _i2e01 + _t2z1 * _i2e02;
  _a2y1 = _t2x1 * _i2e10 + _t2y1 * _i2e11 + _t2z1 * _i2e12;
  _a2z1 = _t2x1 * _i2e20 + _t2y1 * _i2e21 + _t2z1 * _i2e22;

  _t1x2 = _r1y * _az2 - _r1z * _ay2;
  _t1y2 = _r1z * _ax2 - _r1x * _az2;
  _t1z2 = _r1x * _ay2 - _r1y * _ax2;
  _t2x2 = _r2y * _az2 - _r2z * _ay2;
  _t2y2 = _r2z * _ax2 - _r2x * _az2;
  _t2z2 = _r2x * _ay2 - _r2y * _ax2;
  _l1x2 = _ax2 * _m1;
  _l1y2 = _ay2 * _m1;
  _l1z2 = _az2 * _m1;
  _l2x2 = _ax2 * _m2;
  _l2y2 = _ay2 * _m2;
  _l2z2 = _az2 * _m2;
  _a1x2 = _t1x2 * _i1e00 + _t1y2 * _i1e01 + _t1z2 * _i1e02;
  _a1y2 = _t1x2 * _i1e10 + _t1y2 * _i1e11 + _t1z2 * _i1e12;
  _a1z2 = _t1x2 * _i1e20 + _t1y2 * _i1e21 + _t1z2 * _i1e22;
  _a2x2 = _t2x2 * _i2e00 + _t2y2 * _i2e01 + _t2z2 * _i2e02;
  _a2y2 = _t2x2 * _i2e10 + _t2y2 * _i2e11 + _t2z2 * _i2e12;
  _a2z2 = _t2x2 * _i2e20 + _t2y2 * _i2e21 + _t2z2 * _i2e22;

  _t1x3 = _r1y * _az3 - _r1z * _ay3;
  _t1y3 = _r1z * _ax3 - _r1x * _az3;
  _t1z3 = _r1x * _ay3 - _r1y * _ax3;
  _t2x3 = _r2y * _az3 - _r2z * _ay3;
  _t2y3 = _r2z * _ax3 - _r2x * _az3;
  _t2z3 = _r2x * _ay3 - _r2y * _ax3;
  _l1x3 = _ax3 * _m1;
  _l1y3 = _ay3 * _m1;
  _l1z3 = _az3 * _m1;
  _l2x3 = _ax3 * _m2;
  _l2y3 = _ay3 * _m2;
  _l2z3 = _az3 * _m2;
  _a1x3 = _t1x3 * _i1e00 + _t1y3 * _i1e01 + _t1z3 * _i1e02;
  _a1y3 = _t1x3 * _i1e10 + _t1y3 * _i1e11 + _t1z3 * _i1e12;
  _a1z3 = _t1x3 * _i1e20 + _t1y3 * _i1e21 + _t1z3 * _i1e22;
  _a2x3 = _t2x3 * _i2e00 + _t2y3 * _i2e01 + _t2z3 * _i2e02;
  _a2y3 = _t2x3 * _i2e10 + _t2y3 * _i2e11 + _t2z3 * _i2e12;
  _a2z3 = _t2x3 * _i2e20 + _t2y3 * _i2e21 + _t2z3 * _i2e22;

  // build an impulse matrix
  float m12 = _m1 + _m2;
  _k00      = (_ax1 * _ax1 + _ay1 * _ay1 + _az1 * _az1) * m12;
  _k01      = (_ax1 * _ax2 + _ay1 * _ay2 + _az1 * _az2) * m12;
  _k02      = (_ax1 * _ax3 + _ay1 * _ay3 + _az1 * _az3) * m12;
  _k10      = (_ax2 * _ax1 + _ay2 * _ay1 + _az2 * _az1) * m12;
  _k11      = (_ax2 * _ax2 + _ay2 * _ay2 + _az2 * _az2) * m12;
  _k12      = (_ax2 * _ax3 + _ay2 * _ay3 + _az2 * _az3) * m12;
  _k20      = (_ax3 * _ax1 + _ay3 * _ay1 + _az3 * _az1) * m12;
  _k21      = (_ax3 * _ax2 + _ay3 * _ay2 + _az3 * _az2) * m12;
  _k22      = (_ax3 * _ax3 + _ay3 * _ay3 + _az3 * _az3) * m12;

  _k00 += _t1x1 * _a1x1 + _t1y1 * _a1y1 + _t1z1 * _a1z1;
  _k01 += _t1x1 * _a1x2 + _t1y1 * _a1y2 + _t1z1 * _a1z2;
  _k02 += _t1x1 * _a1x3 + _t1y1 * _a1y3 + _t1z1 * _a1z3;
  _k10 += _t1x2 * _a1x1 + _t1y2 * _a1y1 + _t1z2 * _a1z1;
  _k11 += _t1x2 * _a1x2 + _t1y2 * _a1y2 + _t1z2 * _a1z2;
  _k12 += _t1x2 * _a1x3 + _t1y2 * _a1y3 + _t1z2 * _a1z3;
  _k20 += _t1x3 * _a1x1 + _t1y3 * _a1y1 + _t1z3 * _a1z1;
  _k21 += _t1x3 * _a1x2 + _t1y3 * _a1y2 + _t1z3 * _a1z2;
  _k22 += _t1x3 * _a1x3 + _t1y3 * _a1y3 + _t1z3 * _a1z3;

  _k00 += _t2x1 * _a2x1 + _t2y1 * _a2y1 + _t2z1 * _a2z1;
  _k01 += _t2x1 * _a2x2 + _t2y1 * _a2y2 + _t2z1 * _a2z2;
  _k02 += _t2x1 * _a2x3 + _t2y1 * _a2y3 + _t2z1 * _a2z3;
  _k10 += _t2x2 * _a2x1 + _t2y2 * _a2y1 + _t2z2 * _a2z1;
  _k11 += _t2x2 * _a2x2 + _t2y2 * _a2y2 + _t2z2 * _a2z2;
  _k12 += _t2x2 * _a2x3 + _t2y2 * _a2y3 + _t2z2 * _a2z3;
  _k20 += _t2x3 * _a2x1 + _t2y3 * _a2y1 + _t2z3 * _a2z1;
  _k21 += _t2x3 * _a2x2 + _t2y3 * _a2y2 + _t2z3 * _a2z2;
  _k22 += _t2x3 * _a2x3 + _t2y3 * _a2y3 + _t2z3 * _a2z3;

  _kv00 = _k00;
  _kv11 = _k11;
  _kv22 = _k22;

  _dv00 = 1.f / _kv00;
  _dv11 = 1.f / _kv11;
  _dv22 = 1.f / _kv22;

  if (enableSpring1 && _limitState1 != 2) {
    float omega = 6.2831853f * frequency1;
    float k     = omega * omega * timeStep;
    float dmp   = invTimeStep / (k + 2.f * _limitMotor1->dampingRatio * omega);
    _cfm1       = _kv00 * dmp;
    _limitVelocity1 *= k * dmp;
  }
  else {
    _cfm1 = 0.f;
    _limitVelocity1 *= invTimeStep * 0.05f;
  }
  if (enableSpring2 && _limitState2 != 2) {
    float omega = 6.2831853f * frequency2;
    float k     = omega * omega * timeStep;
    float dmp   = invTimeStep / (k + 2.f * _limitMotor2->dampingRatio * omega);
    _cfm2       = _kv11 * dmp;
    _limitVelocity2 *= k * dmp;
  }
  else {
    _cfm2 = 0.f;
    _limitVelocity2 *= invTimeStep * 0.05f;
  }
  if (enableSpring3 && _limitState3 != 2) {
    float omega = 6.2831853f * frequency3;
    float k     = omega * omega * timeStep;
    float dmp   = invTimeStep / (k + 2.f * _limitMotor3->dampingRatio * omega);
    _cfm3       = _kv22 * dmp;
    _limitVelocity3 *= k * dmp;
  }
  else {
    _cfm3 = 0.f;
    _limitVelocity3 *= invTimeStep * 0.05f;
  }
  _k00 += _cfm1;
  _k11 += _cfm2;
  _k22 += _cfm3;

  float inv = 1.f / (_k00 * (_k11 * _k22 - _k21 * _k12)
                     + _k10 * (_k21 * _k02 - _k01 * _k22)
                     + _k20 * (_k01 * _k12 - _k11 * _k02));
  _d00 = (_k11 * _k22 - _k12 * _k21) * inv;
  _d01 = (_k02 * _k21 - _k01 * _k22) * inv;
  _d02 = (_k01 * _k12 - _k02 * _k11) * inv;
  _d10 = (_k12 * _k20 - _k10 * _k22) * inv;
  _d11 = (_k00 * _k22 - _k02 * _k20) * inv;
  _d12 = (_k02 * _k10 - _k00 * _k12) * inv;
  _d20 = (_k10 * _k21 - _k11 * _k20) * inv;
  _d21 = (_k01 * _k20 - _k00 * _k21) * inv;
  _d22 = (_k00 * _k11 - _k01 * _k10) * inv;

  // warm starting
  float totalImpulse1 = _limitImpulse1 + _motorImpulse1;
  float totalImpulse2 = _limitImpulse2 + _motorImpulse2;
  float totalImpulse3 = _limitImpulse3 + _motorImpulse3;
  _l1.x
    += totalImpulse1 * _l1x1 + totalImpulse2 * _l1x2 + totalImpulse3 * _l1x3;
  _l1.y
    += totalImpulse1 * _l1y1 + totalImpulse2 * _l1y2 + totalImpulse3 * _l1y3;
  _l1.z
    += totalImpulse1 * _l1z1 + totalImpulse2 * _l1z2 + totalImpulse3 * _l1z3;
  _a1.x
    += totalImpulse1 * _a1x1 + totalImpulse2 * _a1x2 + totalImpulse3 * _a1x3;
  _a1.y
    += totalImpulse1 * _a1y1 + totalImpulse2 * _a1y2 + totalImpulse3 * _a1y3;
  _a1.z
    += totalImpulse1 * _a1z1 + totalImpulse2 * _a1z2 + totalImpulse3 * _a1z3;
  _l2.x
    -= totalImpulse1 * _l2x1 + totalImpulse2 * _l2x2 + totalImpulse3 * _l2x3;
  _l2.y
    -= totalImpulse1 * _l2y1 + totalImpulse2 * _l2y2 + totalImpulse3 * _l2y3;
  _l2.z
    -= totalImpulse1 * _l2z1 + totalImpulse2 * _l2z2 + totalImpulse3 * _l2z3;
  _a2.x
    -= totalImpulse1 * _a2x1 + totalImpulse2 * _a2x2 + totalImpulse3 * _a2x3;
  _a2.y
    -= totalImpulse1 * _a2y1 + totalImpulse2 * _a2y2 + totalImpulse3 * _a2y3;
  _a2.z
    -= totalImpulse1 * _a2z1 + totalImpulse2 * _a2z2 + totalImpulse3 * _a2z3;
}

void Translational3Constraint::solve()
{
  float rvx
    = _l2.x - _l1.x + _a2.y * _r2z - _a2.z * _r2y - _a1.y * _r1z + _a1.z * _r1y;
  float rvy
    = _l2.y - _l1.y + _a2.z * _r2x - _a2.x * _r2z - _a1.z * _r1x + _a1.x * _r1z;
  float rvz
    = _l2.z - _l1.z + _a2.x * _r2y - _a2.y * _r2x - _a1.x * _r1y + _a1.y * _r1x;
  float rvn1             = rvx * _ax1 + rvy * _ay1 + rvz * _az1;
  float rvn2             = rvx * _ax2 + rvy * _ay2 + rvz * _az2;
  float rvn3             = rvx * _ax3 + rvy * _ay3 + rvz * _az3;
  float oldMotorImpulse1 = _motorImpulse1;
  float oldMotorImpulse2 = _motorImpulse2;
  float oldMotorImpulse3 = _motorImpulse3;
  float dMotorImpulse1   = 0.f;
  float dMotorImpulse2   = 0.f;
  float dMotorImpulse3   = 0.f;
  if (_enableMotor1) {
    dMotorImpulse1 = (rvn1 - _motorSpeed1) * _dv00;
    _motorImpulse1 += dMotorImpulse1;
    if (_motorImpulse1 > _maxMotorImpulse1) { // clamp motor impulse
      _motorImpulse1 = _maxMotorImpulse1;
    }
    else if (_motorImpulse1 < -_maxMotorImpulse1) {
      _motorImpulse1 = -_maxMotorImpulse1;
    }
    dMotorImpulse1 = _motorImpulse1 - oldMotorImpulse1;
  }
  if (_enableMotor2) {
    dMotorImpulse2 = (rvn2 - _motorSpeed2) * _dv11;
    _motorImpulse2 += dMotorImpulse2;
    if (_motorImpulse2 > _maxMotorImpulse2) { // clamp motor impulse
      _motorImpulse2 = _maxMotorImpulse2;
    }
    else if (_motorImpulse2 < -_maxMotorImpulse2) {
      _motorImpulse2 = -_maxMotorImpulse2;
    }
    dMotorImpulse2 = _motorImpulse2 - oldMotorImpulse2;
  }
  if (_enableMotor3) {
    dMotorImpulse3 = (rvn3 - _motorSpeed3) * _dv22;
    _motorImpulse3 += dMotorImpulse3;
    if (_motorImpulse3 > _maxMotorImpulse3) { // clamp motor impulse
      _motorImpulse3 = _maxMotorImpulse3;
    }
    else if (_motorImpulse3 < -_maxMotorImpulse3) {
      _motorImpulse3 = -_maxMotorImpulse3;
    }
    dMotorImpulse3 = _motorImpulse3 - oldMotorImpulse3;
  }

  // apply motor impulse to relative velocity
  rvn1
    += dMotorImpulse1 * _kv00 + dMotorImpulse2 * _k01 + dMotorImpulse3 * _k02;
  rvn2
    += dMotorImpulse1 * _k10 + dMotorImpulse2 * _kv11 + dMotorImpulse3 * _k12;
  rvn3
    += dMotorImpulse1 * _k20 + dMotorImpulse2 * _k21 + dMotorImpulse3 * _kv22;

  // subtract target velocity and applied impulse
  rvn1 -= _limitVelocity1 + _limitImpulse1 * _cfm1;
  rvn2 -= _limitVelocity2 + _limitImpulse2 * _cfm2;
  rvn3 -= _limitVelocity3 + _limitImpulse3 * _cfm3;

  float oldLimitImpulse1 = _limitImpulse1;
  float oldLimitImpulse2 = _limitImpulse2;
  float oldLimitImpulse3 = _limitImpulse3;

  float dLimitImpulse1 = rvn1 * _d00 + rvn2 * _d01 + rvn3 * _d02;
  float dLimitImpulse2 = rvn1 * _d10 + rvn2 * _d11 + rvn3 * _d12;
  float dLimitImpulse3 = rvn1 * _d20 + rvn2 * _d21 + rvn3 * _d22;

  _limitImpulse1 += dLimitImpulse1;
  _limitImpulse2 += dLimitImpulse2;
  _limitImpulse3 += dLimitImpulse3;

  // clamp
  int clampState = 0;
  if (_limitState1 == 2
      || _limitImpulse1 * static_cast<float>(_limitState1) < 0) {
    dLimitImpulse1 = -oldLimitImpulse1;
    rvn2 += dLimitImpulse1 * _k10;
    rvn3 += dLimitImpulse1 * _k20;
    clampState |= 1;
  }
  if (_limitState2 == 2
      || _limitImpulse2 * static_cast<float>(_limitState2) < 0) {
    dLimitImpulse2 = -oldLimitImpulse2;
    rvn1 += dLimitImpulse2 * _k01;
    rvn3 += dLimitImpulse2 * _k21;
    clampState |= 2;
  }
  if (_limitState3 == 2
      || _limitImpulse3 * static_cast<float>(_limitState3) < 0) {
    dLimitImpulse3 = -oldLimitImpulse3;
    rvn1 += dLimitImpulse3 * _k02;
    rvn2 += dLimitImpulse3 * _k12;
    clampState |= 4;
  }

  // update un-clamped impulse
  // TODO: isolate division
  float det;
  switch (clampState) {
    case 1: // update 2 3
      det            = 1.f / (_k11 * _k22 - _k12 * _k21);
      dLimitImpulse2 = (_k22 * rvn2 + -_k12 * rvn3) * det;
      dLimitImpulse3 = (-_k21 * rvn2 + _k11 * rvn3) * det;
      break;
    case 2: // update 1 3
      det            = 1.f / (_k00 * _k22 - _k02 * _k20);
      dLimitImpulse1 = (_k22 * rvn1 + -_k02 * rvn3) * det;
      dLimitImpulse3 = (-_k20 * rvn1 + _k00 * rvn3) * det;
      break;
    case 3: // update 3
      dLimitImpulse3 = rvn3 / _k22;
      break;
    case 4: // update 1 2
      det            = 1.f / (_k00 * _k11 - _k01 * _k10);
      dLimitImpulse1 = (_k11 * rvn1 + -_k01 * rvn2) * det;
      dLimitImpulse2 = (-_k10 * rvn1 + _k00 * rvn2) * det;
      break;
    case 5: // update 2
      dLimitImpulse2 = rvn2 / _k11;
      break;
    case 6: // update 1
      dLimitImpulse1 = rvn1 / _k00;
      break;
    default:
      break;
  }

  _limitImpulse1 = oldLimitImpulse1 + dLimitImpulse1;
  _limitImpulse2 = oldLimitImpulse2 + dLimitImpulse2;
  _limitImpulse3 = oldLimitImpulse3 + dLimitImpulse3;

  float dImpulse1 = dMotorImpulse1 + dLimitImpulse1;
  float dImpulse2 = dMotorImpulse2 + dLimitImpulse2;
  float dImpulse3 = dMotorImpulse3 + dLimitImpulse3;

  // apply impulse
  _l1.x += dImpulse1 * _l1x1 + dImpulse2 * _l1x2 + dImpulse3 * _l1x3;
  _l1.y += dImpulse1 * _l1y1 + dImpulse2 * _l1y2 + dImpulse3 * _l1y3;
  _l1.z += dImpulse1 * _l1z1 + dImpulse2 * _l1z2 + dImpulse3 * _l1z3;
  _a1.x += dImpulse1 * _a1x1 + dImpulse2 * _a1x2 + dImpulse3 * _a1x3;
  _a1.y += dImpulse1 * _a1y1 + dImpulse2 * _a1y2 + dImpulse3 * _a1y3;
  _a1.z += dImpulse1 * _a1z1 + dImpulse2 * _a1z2 + dImpulse3 * _a1z3;
  _l2.x -= dImpulse1 * _l2x1 + dImpulse2 * _l2x2 + dImpulse3 * _l2x3;
  _l2.y -= dImpulse1 * _l2y1 + dImpulse2 * _l2y2 + dImpulse3 * _l2y3;
  _l2.z -= dImpulse1 * _l2z1 + dImpulse2 * _l2z2 + dImpulse3 * _l2z3;
  _a2.x -= dImpulse1 * _a2x1 + dImpulse2 * _a2x2 + dImpulse3 * _a2x3;
  _a2.y -= dImpulse1 * _a2y1 + dImpulse2 * _a2y2 + dImpulse3 * _a2y3;
  _a2.z -= dImpulse1 * _a2z1 + dImpulse2 * _a2z2 + dImpulse3 * _a2z3;
}

} // end of namespace OIMO
