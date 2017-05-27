#include <oimo/constraint/contact/contact_constraint.h>

#include <cmath>

#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/constraint/contact/contact_point_data_buffer.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/math_utils.h>

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
  _cs                   = make_unique<ContactPointDataBuffer>();
  _cs->next             = make_unique<ContactPointDataBuffer>();
  _cs->next->next       = make_unique<ContactPointDataBuffer>();
  _cs->next->next->next = make_unique<ContactPointDataBuffer>();
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
  _p1  = Vec3();
  _p2  = Vec3();
  _lv1 = Vec3();
  _lv2 = Vec3();
  _av1 = Vec3();
  _av2 = Vec3();
  _i1  = Mat33();
  _i2  = Mat33();
}

void ContactConstraint::preSolve(float /*timeStep*/, float invTimeStep)
{
  _m1 = body1->inverseMass;
  _m2 = body2->inverseMass;

  float m1m2 = _m1 + _m2;

  _num = _manifold->numPoints;

  ContactPointDataBuffer* c = _cs.get();
  float rvn, len, norImp, norTar, sepV;
  Mat33 i1, i2;

  for (unsigned int i = 0; i < _num; ++i) {
    const auto& p = _ps[i];

    _tmpP1.sub(p.position, _p1);
    _tmpP2.sub(p.position, _p2);

    _tmpC1.crossVectors(_av1, _tmpP1);
    _tmpC2.crossVectors(_av2, _tmpP2);

    c->norImp = p.normalImpulse;
    c->tanImp = p.tangentImpulse;
    c->binImp = p.binormalImpulse;

    c->nor.copy(p.normal);

    _tmp.set((_lv2.x + _tmpC2.x) - (_lv1.x + _tmpC1.x), //
             (_lv2.y + _tmpC2.y) - (_lv1.y + _tmpC1.y), //
             (_lv2.z + _tmpC2.z) - (_lv1.z + _tmpC1.z));

    rvn = Math::dotVectors(c->nor, _tmp);

    c->tan.set(_tmp.x - rvn * c->nor.x, //
               _tmp.y - rvn * c->nor.y, //
               _tmp.z - rvn * c->nor.z);

    len = Math::dotVectors(c->tan, c->tan);

    if (len <= 0.04f) {
      c->tan.tangent(c->nor);
    }

    c->tan.normalize();

    c->bin.crossVectors(c->nor, c->tan);

    c->norU1.scale(c->nor, _m1);
    c->norU2.scale(c->nor, _m2);

    c->tanU1.scale(c->tan, _m1);
    c->tanU2.scale(c->tan, _m2);

    c->binU1.scale(c->bin, _m1);
    c->binU2.scale(c->bin, _m2);

    c->norT1.crossVectors(_tmpP1, c->nor);
    c->tanT1.crossVectors(_tmpP1, c->tan);
    c->binT1.crossVectors(_tmpP1, c->bin);

    c->norT2.crossVectors(_tmpP2, c->nor);
    c->tanT2.crossVectors(_tmpP2, c->tan);
    c->binT2.crossVectors(_tmpP2, c->bin);

    i1 = _i1;
    i2 = _i2;

    c->norTU1.copy(c->norT1).applyMatrix3(i1, true);
    c->tanTU1.copy(c->tanT1).applyMatrix3(i1, true);
    c->binTU1.copy(c->binT1).applyMatrix3(i1, true);

    c->norTU2.copy(c->norT2).applyMatrix3(i2, true);
    c->tanTU2.copy(c->tanT2).applyMatrix3(i2, true);
    c->binTU2.copy(c->binT2).applyMatrix3(i2, true);

    _tmpC1.crossVectors(c->norTU1, _tmpP1);
    _tmpC2.crossVectors(c->norTU2, _tmpP2);
    _tmp.add(_tmpC1, _tmpC2);
    c->norDen = 1.f / (m1m2 + Math::dotVectors(c->nor, _tmp));

    _tmpC1.crossVectors(c->tanTU1, _tmpP1);
    _tmpC2.crossVectors(c->tanTU2, _tmpP2);
    _tmp.add(_tmpC1, _tmpC2);
    c->tanDen = 1.f / (m1m2 + Math::dotVectors(c->tan, _tmp));

    _tmpC1.crossVectors(c->binTU1, _tmpP1);
    _tmpC2.crossVectors(c->binTU2, _tmpP2);
    _tmp.add(_tmpC1, _tmpC2);
    c->binDen = 1.f / (m1m2 + Math::dotVectors(c->bin, _tmp));

    if (p.warmStarted) {
      norImp = p.normalImpulse;

      _lv1.addScaledVector(c->norU1, norImp);
      _av1.addScaledVector(c->norTU1, norImp);

      _lv2.subScaledVector(c->norU2, norImp);
      _av2.subScaledVector(c->norTU2, norImp);

      c->norImp = norImp;
      c->tanImp = 0.f;
      c->binImp = 0.f;
      rvn       = 0.f; // disable bouncing
    }
    else {
      c->norImp = 0.f;
      c->tanImp = 0.f;
      c->binImp = 0.f;
    }

    if (rvn > -1.f) {
      rvn = 0.f; // disable bouncing
    }

    norTar = restitution * -rvn;
    // allow 0.5cm error
    sepV = -(p.penetration + 0.005f) * invTimeStep * 0.05f;
    if (norTar < sepV) {
      norTar = sepV;
    }
    c->norTar = norTar;
    c->last   = i == _num - 1;
    c         = c->next.get();
  }
}

void ContactConstraint::solve()
{
  _tmplv1.copy(_lv1);
  _tmplv2.copy(_lv2);
  _tmpav1.copy(_av1);
  _tmpav2.copy(_av2);

  float oldImp1, newImp1, oldImp2, newImp2, rvn, norImp, tanImp, binImp, max,
    len;

  ContactPointDataBuffer* c = _cs.get();

  while (true) {
    norImp = c->norImp;
    tanImp = c->tanImp;
    binImp = c->binImp;
    max    = -norImp * friction;

    _tmp.sub(_tmplv2, _tmplv1);

    rvn = Math::dotVectors(_tmp, c->tan) + Math::dotVectors(_tmpav2, c->tanT2)
          - Math::dotVectors(_tmpav1, c->tanT1);

    oldImp1 = tanImp;
    newImp1 = rvn * c->tanDen;
    tanImp += newImp1;

    rvn = Math::dotVectors(_tmp, c->bin) + Math::dotVectors(_tmpav2, c->binT2)
          - Math::dotVectors(_tmpav1, c->binT1);

    oldImp2 = binImp;
    newImp2 = rvn * c->binDen;
    binImp += newImp2;

    // Cone friction clamp
    len = tanImp * tanImp + binImp * binImp;
    if (len > max * max) {
      len = max / std::sqrt(len);
      tanImp *= len;
      binImp *= len;
    }

    newImp1 = tanImp - oldImp1;
    newImp2 = binImp - oldImp2;

    _tmp.set(c->tanU1.x * newImp1 + c->binU1.x * newImp2,  // x
             c->tanU1.y * newImp1 + c->binU1.y * newImp2,  // y
             c->tanU1.z * newImp1 + c->binU1.z * newImp2); // z

    _tmplv1.addEqual(_tmp);

    _tmp.set(c->tanTU1.x * newImp1 + c->binTU1.x * newImp2,  // x
             c->tanTU1.y * newImp1 + c->binTU1.y * newImp2,  // y
             c->tanTU1.z * newImp1 + c->binTU1.z * newImp2); // z

    _tmpav1.addEqual(_tmp);

    _tmp.set(c->tanU2.x * newImp1 + c->binU2.x * newImp2,  // x
             c->tanU2.y * newImp1 + c->binU2.y * newImp2,  // y
             c->tanU2.z * newImp1 + c->binU2.z * newImp2); // z

    _tmplv2.subEqual(_tmp);

    _tmp.set(c->tanTU2.x * newImp1 + c->binTU2.x * newImp2,  // x
             c->tanTU2.y * newImp1 + c->binTU2.y * newImp2,  // y
             c->tanTU2.z * newImp1 + c->binTU2.z * newImp2); // z

    _tmpav2.subEqual(_tmp);

    // restitution part

    _tmp.sub(_tmplv2, _tmplv1);

    rvn = Math::dotVectors(_tmp, c->nor) + Math::dotVectors(_tmpav2, c->norT2)
          - Math::dotVectors(_tmpav1, c->norT1);

    oldImp1 = norImp;
    newImp1 = (rvn - c->norTar) * c->norDen;
    norImp += newImp1;
    if (norImp > 0.f) {
      norImp = 0.f;
    }

    newImp1 = norImp - oldImp1;

    _tmplv1.addScaledVector(c->norU1, newImp1);
    _tmpav1.addScaledVector(c->norTU1, newImp1);
    _tmplv2.subScaledVector(c->norU2, newImp1);
    _tmpav2.subScaledVector(c->norTU2, newImp1);

    c->norImp = norImp;
    c->tanImp = tanImp;
    c->binImp = binImp;

    if (c->last) {
      break;
    }
    c = c->next.get();
  }

  _lv1.copy(_tmplv1);
  _lv2.copy(_tmplv2);
  _av1.copy(_tmpav1);
  _av2.copy(_tmpav2);
}

void ContactConstraint::postSolve()
{
  ContactPointDataBuffer* c = _cs.get();
  for (unsigned int i = _num; i-- > 0;) {
    auto& p = _ps[i];
    p.normal.copy(c->nor);
    p.tangent.copy(c->tan);
    p.binormal.copy(c->bin);

    p.normalImpulse       = c->norImp;
    p.tangentImpulse      = c->tanImp;
    p.binormalImpulse     = c->binImp;
    p.normalDenominator   = c->norDen;
    p.tangentDenominator  = c->tanDen;
    p.binormalDenominator = c->binDen;
    c                     = c->next.get();
  }
}

} // end of namespace OIMO
