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

  std::array<float, 9> v = Mat33().add(_ii1, _ii2).elements;
  inv = 1.f / (v[0] * (v[4] * v[8] - v[7] * v[5])
               + v[3] * (v[7] * v[2] - v[1] * v[8])
               + v[6] * (v[1] * v[5] - v[4] * v[2]));
  _dd = Mat33(v[4] * v[8] - v[5] * v[7], v[2] * v[7] - v[1] * v[8],
              v[1] * v[5] - v[2] * v[4], v[5] * v[6] - v[3] * v[8],
              v[0] * v[8] - v[2] * v[6], v[2] * v[3] - v[0] * v[5],
              v[3] * v[7] - v[4] * v[6], v[1] * v[6] - v[0] * v[7],
              v[0] * v[4] - v[1] * v[3]);
  _dd.multiplyScalar(inv);

  _relativeOrientation.invert(_b1->orientation)
    .multiply(_targetOrientation)
    .multiply(_b2->orientation);

  inv = _relativeOrientation.w * 2.f;

  _vel.scale(_relativeOrientation, inv);

  len = _vel.length();

  if (len > 0.02f) {
    len = (0.02f - len) / len * invTimeStep * 0.05f;
    _vel.multiplyScalar(len);
  }
  else {
    _vel.set(0.f, 0.f, 0.f);
  }

  _rn1.copy(_imp).applyMatrix3(_ii1, true);
  _rn2.copy(_imp).applyMatrix3(_ii2, true);

  _a1.add(_rn1);
  _a2.sub(_rn2);
}

void AngularConstraint::solve()
{
  Vec3 r = _a2.clone().sub(_a1).sub(_vel);

  _rn0.copy(r).applyMatrix3(_dd, true);
  _rn1.copy(_rn0).applyMatrix3(_ii1, true);
  _rn2.copy(_rn0).applyMatrix3(_ii2, true);

  _imp.add(_rn0);
  _a1.add(_rn1);
  _a2.sub(_rn2);
}

} // end of namespace OIMO
