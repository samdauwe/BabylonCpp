#include <oimo/constraint/joint/base/angular_constraint.h>

#include <oimo/constraint/joint/joint.h>
#include <oimo/dynamics/rigid_body.h>

namespace OIMO {

AngularConstraint::AngularConstraint(Joint* joint,
                                     const Quat& targetOrientation)
    : _joint{joint}, _targetOrientation{Quat().invert(targetOrientation)}
{
  _b1 = _joint->body1;
  _b2 = _joint->body2;
  _a1 = _b1->angularVelocity;
  _a2 = _b2->angularVelocity;
  _i1 = _b1->inverseInertia;
  _i2 = _b2->inverseInertia;
}

AngularConstraint::~AngularConstraint()
{
}

void AngularConstraint::preSolve(float /*timeStep*/, float invTimeStep)
{
  float inv, len;

  _ii1 = _i1.clone();
  _ii2 = _i2.clone();

  Mat33 mtx;
  mtx.add(_ii1, _ii2);
  const std::array<float, 9>& v = mtx.elements;

  inv = 1.f / (v[0] * (v[4] * v[8] - v[7] * v[5])
               + v[3] * (v[7] * v[2] - v[1] * v[8])
               + v[6] * (v[1] * v[5] - v[4] * v[2]));
  _dd = Mat33(v[4] * v[8] - v[5] * v[7], v[2] * v[7] - v[1] * v[8],
              v[1] * v[5] - v[2] * v[4], v[5] * v[6] - v[3] * v[8],
              v[0] * v[8] - v[2] * v[6], v[2] * v[3] - v[0] * v[5],
              v[3] * v[7] - v[4] * v[6], v[1] * v[6] - v[0] * v[7],
              v[0] * v[4] - v[1] * v[3]);
  _dd.multiply(inv);

  _relativeOrientation.invert(_b1->orientation);
  _relativeOrientation.mul(_targetOrientation, _relativeOrientation);
  _relativeOrientation.mul(_b2->orientation, _relativeOrientation);
  inv = _relativeOrientation.s * 2.f;

  _vel.scale(_relativeOrientation, inv);

  len = _vel.length();

  if (len > 0.02f) {
    len = (0.02f - len) / len * invTimeStep * 0.05f;
    _vel.scaleEqual(len);
  }
  else {
    _vel.init();
  }

  _rn1.mulMat(_ii1, _imp);
  _rn2.mulMat(_ii2, _imp);

  _a1.addEqual(_rn1);
  _a2.subEqual(_rn2);
}

void AngularConstraint::solve()
{
  Vec3 r = _a2.clone().subEqual(_a1).subEqual(_vel);
  _rn0.mulMat(_dd, r);
  _rn1.mulMat(_ii1, _rn0);
  _rn2.mulMat(_ii2, _rn0);
  _imp.addEqual(_rn0);
  _a1.addEqual(_rn1);
  _a2.subEqual(_rn2);
}

} // end of namespace OIMO
