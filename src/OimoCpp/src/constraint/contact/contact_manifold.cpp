#include <oimo/constraint/contact/contact_manifold.h>

#include <oimo/collision/shape/shape.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/mat33.h>

namespace OIMO {

ContactManifold::ContactManifold()
    : numPoints{0}, body1{nullptr}, body2{nullptr}
{
}

ContactManifold::~ContactManifold()
{
}

void ContactManifold::reset(Shape* shape1, Shape* shape2)
{
  body1     = shape1->parent;
  body2     = shape2->parent;
  numPoints = 0;
}

void ContactManifold::addPointVec(const Vec3& pos, const Vec3& norm,
                                  bool penetration, bool flip)
{
  auto& p = points[numPoints++];

  p.position.copy(pos);
  p.localPoint1.sub(pos, body1->position).applyMatrix3(body1->rotation);
  p.localPoint2.sub(pos, body2->position).applyMatrix3(body2->rotation);

  p.normal.copy(norm);
  if (flip) {
    p.normal.negate();
  }

  p.normalImpulse = 0;
  p.penetration   = penetration;
  p.warmStarted   = false;
}

void ContactManifold::addPoint(float x, float y, float z, float normalX,
                               float normalY, float normalZ, float penetration,
                               bool flip)
{
  auto& p = points[numPoints++];

  p.position.set(x, y, z);
  p.localPoint1.sub(p.position, body1->position).applyMatrix3(body1->rotation);
  p.localPoint2.sub(p.position, body2->position).applyMatrix3(body2->rotation);

  p.normalImpulse = 0;

  p.normal.set(normalX, normalY, normalZ);
  if (flip) {
    p.normal.negate();
  }

  p.penetration = penetration;
  p.warmStarted = false;
}

} // end of namespace OIMO
