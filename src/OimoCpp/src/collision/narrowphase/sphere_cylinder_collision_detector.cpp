#include <oimo/collision/narrowphase/sphere_cylinder_collision_detector.h>

#include <cmath>

#include <oimo/collision/shape/cylinder_shape.h>
#include <oimo/collision/shape/sphere_shape.h>
#include <oimo/constraint/contact/contact_manifold.h>

namespace OIMO {

SphereCylinderCollisionDetector::SphereCylinderCollisionDetector(bool _flip)
    : CollisionDetector{}
{
  flip = _flip;
}

SphereCylinderCollisionDetector::~SphereCylinderCollisionDetector()
{
}

void SphereCylinderCollisionDetector::detectCollision(Shape* shape1,
                                                      Shape* shape2,
                                                      ContactManifold* manifold)
{
  Shape *_s, *_c;
  if (flip) {
    _s = shape2;
    _c = shape1;
  }
  else {
    _s = shape1;
    _c = shape2;
  }

  if (_s->type != Shape::Type::SHAPE_SPHERE
      || _c->type != Shape::Type::SHAPE_CYLINDER) {
    return;
  }
  SphereShape* s   = dynamic_cast<SphereShape*>(_s);
  CylinderShape* c = dynamic_cast<CylinderShape*>(_c);

  const Vec3& ps = s->position;
  float psx      = ps.x;
  float psy      = ps.y;
  float psz      = ps.z;
  const Vec3& pc = c->position;
  float pcx      = pc.x;
  float pcy      = pc.y;
  float pcz      = pc.z;
  float dirx     = c->normalDirection.x;
  float diry     = c->normalDirection.y;
  float dirz     = c->normalDirection.z;
  float rads     = s->radius;
  float radc     = c->radius;
  float rad2     = rads + radc;
  float halfh    = c->halfHeight;
  float dx       = psx - pcx;
  float dy       = psy - pcy;
  float dz       = psz - pcz;
  float dot      = dx * dirx + dy * diry + dz * dirz;
  if (dot < -halfh - rads || dot > halfh + rads) {
    return;
  }
  float cx  = pcx + dot * dirx;
  float cy  = pcy + dot * diry;
  float cz  = pcz + dot * dirz;
  float d2x = psx - cx;
  float d2y = psy - cy;
  float d2z = psz - cz;
  float len = d2x * d2x + d2y * d2y + d2z * d2z;
  if (len > rad2 * rad2) {
    return;
  }
  if (len > radc * radc) {
    len = radc / std::sqrt(len);
    d2x *= len;
    d2y *= len;
    d2z *= len;
  }
  if (dot < -halfh) {
    dot = -halfh;
  }
  else if (dot > halfh) {
    dot = halfh;
  }
  cx  = pcx + dot * dirx + d2x;
  cy  = pcy + dot * diry + d2y;
  cz  = pcz + dot * dirz + d2z;
  dx  = cx - psx;
  dy  = cy - psy;
  dz  = cz - psz;
  len = dx * dx + dy * dy + dz * dz;
  float invLen;
  if (len > 0 && len < rads * rads) {
    len    = std::sqrt(len);
    invLen = 1.f / len;
    dx *= invLen;
    dy *= invLen;
    dz *= invLen;
    manifold->addPoint(psx + dx * rads, psy + dy * rads, psz + dz * rads, dx,
                       dy, dz, len - rads, flip);
  }
}

} // end of namespace OIMO
