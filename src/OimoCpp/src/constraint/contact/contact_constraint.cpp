#include <oimo/constraint/contact/contact_constraint.h>

#include <cmath>

#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/constraint/contact/contact_point_data_buffer.h>
#include <oimo/dynamics/rigid_body.h>

namespace OIMO {

ContactConstraint::ContactConstraint(ContactManifold* manifold)
    : Constraint{}
    , restitution{0.f}
    , friction{0.f}
    , _manifold{manifold}
    , _m1{0.f}
    , _m2{0.f}
    , _num{0}
{
  _ps                   = _manifold->points;
  _cs                   = new ContactPointDataBuffer();
  _cs->next             = new ContactPointDataBuffer();
  _cs->next->next       = new ContactPointDataBuffer();
  _cs->next->next->next = new ContactPointDataBuffer();
}

ContactConstraint::~ContactConstraint()
{
}

void ContactConstraint::attach()
{
  _p1  = body1->position;
  _p2  = body2->position;
  _lv1 = body1->linearVelocity;
  _av1 = body1->angularVelocity;
  _lv2 = body2->linearVelocity;
  _av2 = body2->angularVelocity;
  _i1  = body1->inverseInertia;
  _i2  = body2->inverseInertia;
}

void ContactConstraint::detach()
{
}

void ContactConstraint::preSolve(float /*timeStep*/, float invTimeStep)
{
  _m1 = body1->inverseMass;
  _m2 = body2->inverseMass;

  _ii1 = _i1.clone();
  _ii2 = _i2.clone();

  std::array<float, 9>& ii1 = _ii1.elements;
  std::array<float, 9>& ii2 = _ii2.elements;

  float p1x = _p1.x;
  float p1y = _p1.y;
  float p1z = _p1.z;
  float p2x = _p2.x;
  float p2y = _p2.y;
  float p2z = _p2.z;

  float m1m2 = _m1 + _m2;

  _num = _manifold->numPoints;

  ContactPointDataBuffer* c = _cs;

  for (unsigned int i = 0; i < _num; i++) {
    ManifoldPoint& p = _ps[i];
    float tmp1X;
    float tmp1Y;
    float tmp1Z;
    float tmp2X;
    float tmp2Y;
    float tmp2Z;
    tmp1X      = p.position.x;
    tmp1Y      = p.position.y;
    tmp1Z      = p.position.z;
    float rp1X = tmp1X - p1x;
    float rp1Y = tmp1Y - p1y;
    float rp1Z = tmp1Z - p1z;
    float rp2X = tmp1X - p2x;
    float rp2Y = tmp1Y - p2y;
    float rp2Z = tmp1Z - p2z;
    c->rp1X    = rp1X;
    c->rp1Y    = rp1Y;
    c->rp1Z    = rp1Z;
    c->rp2X    = rp2X;
    c->rp2Y    = rp2Y;
    c->rp2Z    = rp2Z;
    c->norImp  = p.normalImpulse;
    c->tanImp  = p.tangentImpulse;
    c->binImp  = p.binormalImpulse;
    float norX = p.normal.x;
    float norY = p.normal.y;
    float norZ = p.normal.z;
    float rvX  = (_lv2.x + _av2.y * rp2Z - _av2.z * rp2Y)
                - (_lv1.x + _av1.y * rp1Z - _av1.z * rp1Y);
    float rvY = (_lv2.y + _av2.z * rp2X - _av2.x * rp2Z)
                - (_lv1.y + _av1.z * rp1X - _av1.x * rp1Z);
    float rvZ = (_lv2.z + _av2.x * rp2Y - _av2.y * rp2X)
                - (_lv1.z + _av1.x * rp1Y - _av1.y * rp1X);
    float rvn  = norX * rvX + norY * rvY + norZ * rvZ;
    float tanX = rvX - rvn * norX;
    float tanY = rvY - rvn * norY;
    float tanZ = rvZ - rvn * norZ;
    float len  = tanX * tanX + tanY * tanY + tanZ * tanZ;
    if (len > 0.04f) {
      len = 1.f / std::sqrt(len);
    }
    else {
      tanX = norY * norX - norZ * norZ;
      tanY = -norZ * norY - norX * norX;
      tanZ = norX * norZ + norY * norY;
      len  = 1.f / std::sqrt(tanX * tanX + tanY * tanY + tanZ * tanZ);
    }
    tanX *= len;
    tanY *= len;
    tanZ *= len;
    float binX    = norY * tanZ - norZ * tanY;
    float binY    = norZ * tanX - norX * tanZ;
    float binZ    = norX * tanY - norY * tanX;
    c->norX       = norX;
    c->norY       = norY;
    c->norZ       = norZ;
    c->tanX       = tanX;
    c->tanY       = tanY;
    c->tanZ       = tanZ;
    c->binX       = binX;
    c->binY       = binY;
    c->binZ       = binZ;
    c->norU1X     = norX * _m1;
    c->norU1Y     = norY * _m1;
    c->norU1Z     = norZ * _m1;
    c->norU2X     = norX * _m2;
    c->norU2Y     = norY * _m2;
    c->norU2Z     = norZ * _m2;
    c->tanU1X     = tanX * _m1;
    c->tanU1Y     = tanY * _m1;
    c->tanU1Z     = tanZ * _m1;
    c->tanU2X     = tanX * _m2;
    c->tanU2Y     = tanY * _m2;
    c->tanU2Z     = tanZ * _m2;
    c->binU1X     = binX * _m1;
    c->binU1Y     = binY * _m1;
    c->binU1Z     = binZ * _m1;
    c->binU2X     = binX * _m2;
    c->binU2Y     = binY * _m2;
    c->binU2Z     = binZ * _m2;
    float norT1X  = rp1Y * norZ - rp1Z * norY;
    float norT1Y  = rp1Z * norX - rp1X * norZ;
    float norT1Z  = rp1X * norY - rp1Y * norX;
    float norT2X  = rp2Y * norZ - rp2Z * norY;
    float norT2Y  = rp2Z * norX - rp2X * norZ;
    float norT2Z  = rp2X * norY - rp2Y * norX;
    float tanT1X  = rp1Y * tanZ - rp1Z * tanY;
    float tanT1Y  = rp1Z * tanX - rp1X * tanZ;
    float tanT1Z  = rp1X * tanY - rp1Y * tanX;
    float tanT2X  = rp2Y * tanZ - rp2Z * tanY;
    float tanT2Y  = rp2Z * tanX - rp2X * tanZ;
    float tanT2Z  = rp2X * tanY - rp2Y * tanX;
    float binT1X  = rp1Y * binZ - rp1Z * binY;
    float binT1Y  = rp1Z * binX - rp1X * binZ;
    float binT1Z  = rp1X * binY - rp1Y * binX;
    float binT2X  = rp2Y * binZ - rp2Z * binY;
    float binT2Y  = rp2Z * binX - rp2X * binZ;
    float binT2Z  = rp2X * binY - rp2Y * binX;
    float norTU1X = norT1X * ii1[0] + norT1Y * ii1[1] + norT1Z * ii1[2];
    float norTU1Y = norT1X * ii1[3] + norT1Y * ii1[4] + norT1Z * ii1[5];
    float norTU1Z = norT1X * ii1[6] + norT1Y * ii1[7] + norT1Z * ii1[8];
    float norTU2X = norT2X * ii2[0] + norT2Y * ii2[1] + norT2Z * ii2[2];
    float norTU2Y = norT2X * ii2[3] + norT2Y * ii2[4] + norT2Z * ii2[5];
    float norTU2Z = norT2X * ii2[6] + norT2Y * ii2[7] + norT2Z * ii2[8];
    float tanTU1X = tanT1X * ii1[0] + tanT1Y * ii1[1] + tanT1Z * ii1[2];
    float tanTU1Y = tanT1X * ii1[3] + tanT1Y * ii1[4] + tanT1Z * ii1[5];
    float tanTU1Z = tanT1X * ii1[6] + tanT1Y * ii1[7] + tanT1Z * ii1[8];
    float tanTU2X = tanT2X * ii2[0] + tanT2Y * ii2[1] + tanT2Z * ii2[2];
    float tanTU2Y = tanT2X * ii2[3] + tanT2Y * ii2[4] + tanT2Z * ii2[5];
    float tanTU2Z = tanT2X * ii2[6] + tanT2Y * ii2[7] + tanT2Z * ii2[8];
    float binTU1X = binT1X * ii1[0] + binT1Y * ii1[1] + binT1Z * ii1[2];
    float binTU1Y = binT1X * ii1[3] + binT1Y * ii1[4] + binT1Z * ii1[5];
    float binTU1Z = binT1X * ii1[6] + binT1Y * ii1[7] + binT1Z * ii1[8];
    float binTU2X = binT2X * ii2[0] + binT2Y * ii2[1] + binT2Z * ii2[2];
    float binTU2Y = binT2X * ii2[3] + binT2Y * ii2[4] + binT2Z * ii2[5];
    float binTU2Z = binT2X * ii2[6] + binT2Y * ii2[7] + binT2Z * ii2[8];
    c->norT1X     = norT1X;
    c->norT1Y     = norT1Y;
    c->norT1Z     = norT1Z;
    c->tanT1X     = tanT1X;
    c->tanT1Y     = tanT1Y;
    c->tanT1Z     = tanT1Z;
    c->binT1X     = binT1X;
    c->binT1Y     = binT1Y;
    c->binT1Z     = binT1Z;
    c->norT2X     = norT2X;
    c->norT2Y     = norT2Y;
    c->norT2Z     = norT2Z;
    c->tanT2X     = tanT2X;
    c->tanT2Y     = tanT2Y;
    c->tanT2Z     = tanT2Z;
    c->binT2X     = binT2X;
    c->binT2Y     = binT2Y;
    c->binT2Z     = binT2Z;
    c->norTU1X    = norTU1X;
    c->norTU1Y    = norTU1Y;
    c->norTU1Z    = norTU1Z;
    c->tanTU1X    = tanTU1X;
    c->tanTU1Y    = tanTU1Y;
    c->tanTU1Z    = tanTU1Z;
    c->binTU1X    = binTU1X;
    c->binTU1Y    = binTU1Y;
    c->binTU1Z    = binTU1Z;
    c->norTU2X    = norTU2X;
    c->norTU2Y    = norTU2Y;
    c->norTU2Z    = norTU2Z;
    c->tanTU2X    = tanTU2X;
    c->tanTU2Y    = tanTU2Y;
    c->tanTU2Z    = tanTU2Z;
    c->binTU2X    = binTU2X;
    c->binTU2Y    = binTU2Y;
    c->binTU2Z    = binTU2Z;
    tmp1X         = norT1X * ii1[0] + norT1Y * ii1[1] + norT1Z * ii1[2];
    tmp1Y         = norT1X * ii1[3] + norT1Y * ii1[4] + norT1Z * ii1[5];
    tmp1Z         = norT1X * ii1[6] + norT1Y * ii1[7] + norT1Z * ii1[8];
    tmp2X         = tmp1Y * rp1Z - tmp1Z * rp1Y;
    tmp2Y         = tmp1Z * rp1X - tmp1X * rp1Z;
    tmp2Z         = tmp1X * rp1Y - tmp1Y * rp1X;
    tmp1X         = norT2X * ii2[0] + norT2Y * ii2[1] + norT2Z * ii2[2];
    tmp1Y         = norT2X * ii2[3] + norT2Y * ii2[4] + norT2Z * ii2[5];
    tmp1Z         = norT2X * ii2[6] + norT2Y * ii2[7] + norT2Z * ii2[8];
    tmp2X += tmp1Y * rp2Z - tmp1Z * rp2Y;
    tmp2Y += tmp1Z * rp2X - tmp1X * rp2Z;
    tmp2Z += tmp1X * rp2Y - tmp1Y * rp2X;
    float norDen = 1 / (m1m2 + norX * tmp2X + norY * tmp2Y + norZ * tmp2Z);
    tmp1X        = tanT1X * ii1[0] + tanT1Y * ii1[1] + tanT1Z * ii1[2];
    tmp1Y        = tanT1X * ii1[3] + tanT1Y * ii1[4] + tanT1Z * ii1[5];
    tmp1Z        = tanT1X * ii1[6] + tanT1Y * ii1[7] + tanT1Z * ii1[8];
    tmp2X        = tmp1Y * rp1Z - tmp1Z * rp1Y;
    tmp2Y        = tmp1Z * rp1X - tmp1X * rp1Z;
    tmp2Z        = tmp1X * rp1Y - tmp1Y * rp1X;
    tmp1X        = tanT2X * ii2[0] + tanT2Y * ii2[1] + tanT2Z * ii2[2];
    tmp1Y        = tanT2X * ii2[3] + tanT2Y * ii2[4] + tanT2Z * ii2[5];
    tmp1Z        = tanT2X * ii2[6] + tanT2Y * ii2[7] + tanT2Z * ii2[8];
    tmp2X += tmp1Y * rp2Z - tmp1Z * rp2Y;
    tmp2Y += tmp1Z * rp2X - tmp1X * rp2Z;
    tmp2Z += tmp1X * rp2Y - tmp1Y * rp2X;
    float tanDen = 1.f / (m1m2 + tanX * tmp2X + tanY * tmp2Y + tanZ * tmp2Z);
    tmp1X        = binT1X * ii1[0] + binT1Y * ii1[1] + binT1Z * ii1[2];
    tmp1Y        = binT1X * ii1[3] + binT1Y * ii1[4] + binT1Z * ii1[5];
    tmp1Z        = binT1X * ii1[6] + binT1Y * ii1[7] + binT1Z * ii1[8];
    tmp2X        = tmp1Y * rp1Z - tmp1Z * rp1Y;
    tmp2Y        = tmp1Z * rp1X - tmp1X * rp1Z;
    tmp2Z        = tmp1X * rp1Y - tmp1Y * rp1X;
    tmp1X        = binT2X * ii2[0] + binT2Y * ii2[1] + binT2Z * ii2[2];
    tmp1Y        = binT2X * ii2[3] + binT2Y * ii2[4] + binT2Z * ii2[5];
    tmp1Z        = binT2X * ii2[6] + binT2Y * ii2[7] + binT2Z * ii2[8];
    tmp2X += tmp1Y * rp2Z - tmp1Z * rp2Y;
    tmp2Y += tmp1Z * rp2X - tmp1X * rp2Z;
    tmp2Z += tmp1X * rp2Y - tmp1Y * rp2X;
    float binDen = 1.f / (m1m2 + binX * tmp2X + binY * tmp2Y + binZ * tmp2Z);
    c->norDen    = norDen;
    c->tanDen    = tanDen;
    c->binDen    = binDen;
    if (p.warmStarted) {
      float norImp = p.normalImpulse;
      _lv1.x += c->norU1X * norImp;
      _lv1.y += c->norU1Y * norImp;
      _lv1.z += c->norU1Z * norImp;
      _av1.x += norTU1X * norImp;
      _av1.y += norTU1Y * norImp;
      _av1.z += norTU1Z * norImp;
      _lv2.x -= c->norU2X * norImp;
      _lv2.y -= c->norU2Y * norImp;
      _lv2.z -= c->norU2Z * norImp;
      _av2.x -= norTU2X * norImp;
      _av2.y -= norTU2Y * norImp;
      _av2.z -= norTU2Z * norImp;
      c->norImp = norImp;
      c->tanImp = 0;
      c->binImp = 0;
      rvn       = 0; // disable bouncing
    }
    else {
      c->norImp = 0;
      c->tanImp = 0;
      c->binImp = 0;
    }

    if (rvn > -1) {
      rvn = 0; // disable bouncing
    }
    float norTar = restitution * -rvn;
    float sepV
      = -(p.penetration + 0.005f) * invTimeStep * 0.05f; // allow 0.5cm error
    if (norTar < sepV) {
      norTar = sepV;
    }
    c->norTar = norTar;
    c->last   = (i == _num - 1);
    c         = c->next;
  }
}

void ContactConstraint::solve()
{
  float lv1x                = _lv1.x;
  float lv1y                = _lv1.y;
  float lv1z                = _lv1.z;
  float lv2x                = _lv2.x;
  float lv2y                = _lv2.y;
  float lv2z                = _lv2.z;
  float av1x                = _av1.x;
  float av1y                = _av1.y;
  float av1z                = _av1.z;
  float av2x                = _av2.x;
  float av2y                = _av2.y;
  float av2z                = _av2.z;
  ContactPointDataBuffer* c = _cs;
  while (true) {
    float oldImp1;
    float newImp1;
    float oldImp2;
    float newImp2;
    float rvn;
    float norImp = c->norImp;
    float tanImp = c->tanImp;
    float binImp = c->binImp;
    float max    = -norImp * friction;
    float rvX    = lv2x - lv1x;
    float rvY    = lv2y - lv1y;
    float rvZ    = lv2z - lv1z;
    rvn = rvX * c->tanX + rvY * c->tanY + rvZ * c->tanZ + av2x * c->tanT2X
          + av2y * c->tanT2Y + av2z * c->tanT2Z - av1x * c->tanT1X
          - av1y * c->tanT1Y - av1z * c->tanT1Z;
    oldImp1 = tanImp;
    newImp1 = rvn * c->tanDen;
    tanImp += newImp1;
    rvn = rvX * c->binX + rvY * c->binY + rvZ * c->binZ + av2x * c->binT2X
          + av2y * c->binT2Y + av2z * c->binT2Z - av1x * c->binT1X
          - av1y * c->binT1Y - av1z * c->binT1Z;
    oldImp2 = binImp;
    newImp2 = rvn * c->binDen;
    binImp += newImp2;

    // cone friction clamp
    float len = tanImp * tanImp + binImp * binImp;
    if (len > max * max) {
      len = max / std::sqrt(len);
      tanImp *= len;
      binImp *= len;
    }
    newImp1 = tanImp - oldImp1;
    newImp2 = binImp - oldImp2;

    lv1x += c->tanU1X * newImp1 + c->binU1X * newImp2;
    lv1y += c->tanU1Y * newImp1 + c->binU1Y * newImp2;
    lv1z += c->tanU1Z * newImp1 + c->binU1Z * newImp2;
    av1x += c->tanTU1X * newImp1 + c->binTU1X * newImp2;
    av1y += c->tanTU1Y * newImp1 + c->binTU1Y * newImp2;
    av1z += c->tanTU1Z * newImp1 + c->binTU1Z * newImp2;
    lv2x -= c->tanU2X * newImp1 + c->binU2X * newImp2;
    lv2y -= c->tanU2Y * newImp1 + c->binU2Y * newImp2;
    lv2z -= c->tanU2Z * newImp1 + c->binU2Z * newImp2;
    av2x -= c->tanTU2X * newImp1 + c->binTU2X * newImp2;
    av2y -= c->tanTU2Y * newImp1 + c->binTU2Y * newImp2;
    av2z -= c->tanTU2Z * newImp1 + c->binTU2Z * newImp2;

    // restitution part
    rvn = (lv2x - lv1x) * c->norX + (lv2y - lv1y) * c->norY
          + (lv2z - lv1z) * c->norZ + av2x * c->norT2X + av2y * c->norT2Y
          + av2z * c->norT2Z - av1x * c->norT1X - av1y * c->norT1Y
          - av1z * c->norT1Z;

    oldImp1 = norImp;
    newImp1 = (rvn - c->norTar) * c->norDen;
    norImp += newImp1;
    if (norImp > 0.f) {
      norImp = 0.f;
    }
    newImp1 = norImp - oldImp1;
    lv1x += c->norU1X * newImp1;
    lv1y += c->norU1Y * newImp1;
    lv1z += c->norU1Z * newImp1;
    av1x += c->norTU1X * newImp1;
    av1y += c->norTU1Y * newImp1;
    av1z += c->norTU1Z * newImp1;
    lv2x -= c->norU2X * newImp1;
    lv2y -= c->norU2Y * newImp1;
    lv2z -= c->norU2Z * newImp1;
    av2x -= c->norTU2X * newImp1;
    av2y -= c->norTU2Y * newImp1;
    av2z -= c->norTU2Z * newImp1;

    c->norImp = norImp;
    c->tanImp = tanImp;
    c->binImp = binImp;

    if (c->last)
      break;
    c = c->next;
  }
  _lv1.x = lv1x;
  _lv1.y = lv1y;
  _lv1.z = lv1z;
  _lv2.x = lv2x;
  _lv2.y = lv2y;
  _lv2.z = lv2z;
  _av1.x = av1x;
  _av1.y = av1y;
  _av1.z = av1z;
  _av2.x = av2x;
  _av2.y = av2y;
  _av2.z = av2z;
}

void ContactConstraint::postSolve()
{
  ContactPointDataBuffer* c = _cs;
  for (unsigned int i = _num + 1; i > 0; --i) {
    ManifoldPoint& p      = _ps[i - 1];
    p.normal.x            = c->norX;
    p.normal.y            = c->norY;
    p.normal.z            = c->norZ;
    p.tangent.x           = c->tanX;
    p.tangent.y           = c->tanY;
    p.tangent.z           = c->tanZ;
    p.binormal.x          = c->binX;
    p.binormal.y          = c->binY;
    p.binormal.z          = c->binZ;
    p.normalImpulse       = c->norImp;
    p.tangentImpulse      = c->tanImp;
    p.binormalImpulse     = c->binImp;
    p.normalDenominator   = c->norDen;
    p.tangentDenominator  = c->tanDen;
    p.binormalDenominator = c->binDen;
    c                     = c->next;
  }
}

} // end of namespace OIMO
