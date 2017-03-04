#include <oimo/constraint/joint/base/linear_constraint.h>

#include <cmath>

#include <oimo/constraint/joint/joint.h>
#include <oimo/dynamics/rigid_body.h>

namespace OIMO {

LinearConstraint::LinearConstraint(Joint* joint)
    : _joint{joint}, _impx{0.f}, _impy{0.f}, _impz{0.f}
{
  _r1 = _joint->relativeAnchorPoint1;
  _r2 = _joint->relativeAnchorPoint2;
  _p1 = _joint->anchorPoint1;
  _p2 = _joint->anchorPoint2;
  _b1 = _joint->body1;
  _b2 = _joint->body2;
  _l1 = _b1->linearVelocity;
  _l2 = _b2->linearVelocity;
  _a1 = _b1->angularVelocity;
  _a2 = _b2->angularVelocity;
  _i1 = _b1->inverseInertia;
  _i2 = _b2->inverseInertia;
}

LinearConstraint::~LinearConstraint()
{
}

void LinearConstraint::preSolve(float /*timeStep*/, float invTimeStep)
{
  _r1x = _r1.x;
  _r1y = _r1.y;
  _r1z = _r1.z;

  _r2x = _r2.x;
  _r2y = _r2.y;
  _r2z = _r2.z;

  _m1 = _b1->inverseMass;
  _m2 = _b2->inverseMass;

  _ii1 = _i1.clone();
  _ii2 = _i2.clone();

  const std::array<float, 9>& ii1 = _ii1.elements;
  const std::array<float, 9>& ii2 = _ii2.elements;

  _ax1x = _r1z * ii1[1] + -_r1y * ii1[2];
  _ax1y = _r1z * ii1[4] + -_r1y * ii1[5];
  _ax1z = _r1z * ii1[7] + -_r1y * ii1[8];
  _ay1x = -_r1z * ii1[0] + _r1x * ii1[2];
  _ay1y = -_r1z * ii1[3] + _r1x * ii1[5];
  _ay1z = -_r1z * ii1[6] + _r1x * ii1[8];
  _az1x = _r1y * ii1[0] + -_r1x * ii1[1];
  _az1y = _r1y * ii1[3] + -_r1x * ii1[4];
  _az1z = _r1y * ii1[6] + -_r1x * ii1[7];
  _ax2x = _r2z * ii2[1] + -_r2y * ii2[2];
  _ax2y = _r2z * ii2[4] + -_r2y * ii2[5];
  _ax2z = _r2z * ii2[7] + -_r2y * ii2[8];
  _ay2x = -_r2z * ii2[0] + _r2x * ii2[2];
  _ay2y = -_r2z * ii2[3] + _r2x * ii2[5];
  _ay2z = -_r2z * ii2[6] + _r2x * ii2[8];
  _az2x = _r2y * ii2[0] + -_r2x * ii2[1];
  _az2y = _r2y * ii2[3] + -_r2x * ii2[4];
  _az2z = _r2y * ii2[6] + -_r2x * ii2[7];

  // calculate point-to-point mass matrix
  // from impulse equation
  //
  // M = ([/m] - [r^][/I][r^]) ^ -1
  //
  // where
  //
  // [/m] = |1/m, 0, 0|
  //        |0, 1/m, 0|
  //        |0, 0, 1/m|
  //
  // [r^] = |0, -rz, ry|
  //        |rz, 0, -rx|
  //        |-ry, rx, 0|
  //
  // [/I] = Inverted moment inertia

  float rxx = _m1 + _m2;

  Mat33 kk(rxx, 0.f, 0.f, 0.f, rxx, 0.f, 0.f, 0.f, rxx);
  std::array<float, 9>& k = kk.elements;

  k[0] += ii1[4] * _r1z * _r1z - (ii1[7] + ii1[5]) * _r1y * _r1z
          + ii1[8] * _r1y * _r1y;
  k[1] += (ii1[6] * _r1y + ii1[5] * _r1x) * _r1z - ii1[3] * _r1z * _r1z
          - ii1[8] * _r1x * _r1y;
  k[2] += (ii1[3] * _r1y - ii1[4] * _r1x) * _r1z - ii1[6] * _r1y * _r1y
          + ii1[7] * _r1x * _r1y;
  k[3] += (ii1[2] * _r1y + ii1[7] * _r1x) * _r1z - ii1[1] * _r1z * _r1z
          - ii1[8] * _r1x * _r1y;
  k[4] += ii1[0] * _r1z * _r1z - (ii1[6] + ii1[2]) * _r1x * _r1z
          + ii1[8] * _r1x * _r1x;
  k[5] += (ii1[1] * _r1x - ii1[0] * _r1y) * _r1z - ii1[7] * _r1x * _r1x
          + ii1[6] * _r1x * _r1y;
  k[6] += (ii1[1] * _r1y - ii1[4] * _r1x) * _r1z - ii1[2] * _r1y * _r1y
          + ii1[5] * _r1x * _r1y;
  k[7] += (ii1[3] * _r1x - ii1[0] * _r1y) * _r1z - ii1[5] * _r1x * _r1x
          + ii1[2] * _r1x * _r1y;
  k[8] += ii1[0] * _r1y * _r1y - (ii1[3] + ii1[1]) * _r1x * _r1y
          + ii1[4] * _r1x * _r1x;

  k[0] += ii2[4] * _r2z * _r2z - (ii2[7] + ii2[5]) * _r2y * _r2z
          + ii2[8] * _r2y * _r2y;
  k[1] += (ii2[6] * _r2y + ii2[5] * _r2x) * _r2z - ii2[3] * _r2z * _r2z
          - ii2[8] * _r2x * _r2y;
  k[2] += (ii2[3] * _r2y - ii2[4] * _r2x) * _r2z - ii2[6] * _r2y * _r2y
          + ii2[7] * _r2x * _r2y;
  k[3] += (ii2[2] * _r2y + ii2[7] * _r2x) * _r2z - ii2[1] * _r2z * _r2z
          - ii2[8] * _r2x * _r2y;
  k[4] += ii2[0] * _r2z * _r2z - (ii2[6] + ii2[2]) * _r2x * _r2z
          + ii2[8] * _r2x * _r2x;
  k[5] += (ii2[1] * _r2x - ii2[0] * _r2y) * _r2z - ii2[7] * _r2x * _r2x
          + ii2[6] * _r2x * _r2y;
  k[6] += (ii2[1] * _r2y - ii2[4] * _r2x) * _r2z - ii2[2] * _r2y * _r2y
          + ii2[5] * _r2x * _r2y;
  k[7] += (ii2[3] * _r2x - ii2[0] * _r2y) * _r2z - ii2[5] * _r2x * _r2x
          + ii2[2] * _r2x * _r2y;
  k[8] += ii2[0] * _r2y * _r2y - (ii2[3] + ii2[1]) * _r2x * _r2y
          + ii2[4] * _r2x * _r2x;

  float inv = 1.f / (k[0] * (k[4] * k[8] - k[7] * k[5])
                     + k[3] * (k[7] * k[2] - k[1] * k[8])
                     + k[6] * (k[1] * k[5] - k[4] * k[2]));
  _dd = Mat33(k[4] * k[8] - k[5] * k[7], k[2] * k[7] - k[1] * k[8],
              k[1] * k[5] - k[2] * k[4], k[5] * k[6] - k[3] * k[8],
              k[0] * k[8] - k[2] * k[6], k[2] * k[3] - k[0] * k[5],
              k[3] * k[7] - k[4] * k[6], k[1] * k[6] - k[0] * k[7],
              k[0] * k[4] - k[1] * k[3]).scaleEqual( inv );

  _velx     = _p2.x - _p1.x;
  _vely     = _p2.y - _p1.y;
  _velz     = _p2.z - _p1.z;
  float len = std::sqrt(_velx * _velx + _vely * _vely + _velz * _velz);
  if (len > 0.005f) {
    len = (0.005f - len) / len * invTimeStep * 0.05f;
    _velx *= len;
    _vely *= len;
    _velz *= len;
  }
  else {
    _velx = 0.f;
    _vely = 0.f;
    _velz = 0.f;
  }

  _impx *= 0.95f;
  _impy *= 0.95f;
  _impz *= 0.95f;

  _l1.x += _impx * _m1;
  _l1.y += _impy * _m1;
  _l1.z += _impz * _m1;
  _a1.x += _impx * _ax1x + _impy * _ay1x + _impz * _az1x;
  _a1.y += _impx * _ax1y + _impy * _ay1y + _impz * _az1y;
  _a1.z += _impx * _ax1z + _impy * _ay1z + _impz * _az1z;
  _l2.x -= _impx * _m2;
  _l2.y -= _impy * _m2;
  _l2.z -= _impz * _m2;
  _a2.x -= _impx * _ax2x + _impy * _ay2x + _impz * _az2x;
  _a2.y -= _impx * _ax2y + _impy * _ay2y + _impz * _az2y;
  _a2.z -= _impx * _ax2z + _impy * _ay2z + _impz * _az2z;
}

void LinearConstraint::solve()
{
  const std::array<float, 9>& d = _dd.elements;
  float rvx = _l2.x - _l1.x + _a2.y * _r2z - _a2.z * _r2y - _a1.y * _r1z
              + _a1.z * _r1y - _velx;
  float rvy = _l2.y - _l1.y + _a2.z * _r2x - _a2.x * _r2z - _a1.z * _r1x
              + _a1.x * _r1z - _vely;
  float rvz = _l2.z - _l1.z + _a2.x * _r2y - _a2.y * _r2x - _a1.x * _r1y
              + _a1.y * _r1x - _velz;
  float nimpx = rvx * d[0] + rvy * d[1] + rvz * d[2];
  float nimpy = rvx * d[3] + rvy * d[4] + rvz * d[5];
  float nimpz = rvx * d[6] + rvy * d[7] + rvz * d[8];
  _impx += nimpx;
  _impy += nimpy;
  _impz += nimpz;
  _l1.x += nimpx * _m1;
  _l1.y += nimpy * _m1;
  _l1.z += nimpz * _m1;
  _a1.x += nimpx * _ax1x + nimpy * _ay1x + nimpz * _az1x;
  _a1.y += nimpx * _ax1y + nimpy * _ay1y + nimpz * _az1y;
  _a1.z += nimpx * _ax1z + nimpy * _ay1z + nimpz * _az1z;
  _l2.x -= nimpx * _m2;
  _l2.y -= nimpy * _m2;
  _l2.z -= nimpz * _m2;
  _a2.x -= nimpx * _ax2x + nimpy * _ay2x + nimpz * _az2x;
  _a2.y -= nimpx * _ax2y + nimpy * _ay2y + nimpz * _az2y;
  _a2.z -= nimpx * _ax2z + nimpy * _ay2z + nimpz * _az2z;
}

} // end of namespace OIMO
