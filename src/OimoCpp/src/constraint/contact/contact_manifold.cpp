#include <oimo/constraint/contact/contact_manifold.h>

#include <oimo/collision/shape/shape.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/mat33.h>

namespace OIMO {

ContactManifold::ContactManifold()
    : numPoints{0}, body1{new RigidBody()}, body2{new RigidBody()}
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

void ContactManifold::addPoint(float x, float y, float z, float normalX,
                               float normalY, float normalZ, float penetration,
                               bool flip)
{
  ManifoldPoint& p = points[numPoints++];
  p.position.x     = x;
  p.position.y     = y;
  p.position.z     = z;
  Mat33 r          = body1->rotation;
  float rx         = x - body1->position.x;
  float ry         = y - body1->position.y;
  float rz         = z - body1->position.z;

  std::array<float, 9>& tr = r.elements;
  p.localPoint1.x = rx * tr[0] + ry * tr[3] + rz * tr[6];
  p.localPoint1.y = rx * tr[1] + ry * tr[4] + rz * tr[7];
  p.localPoint1.z = rx * tr[2] + ry * tr[5] + rz * tr[8];
  r               = body2->rotation;
  rx              = x - body2->position.x;
  ry              = y - body2->position.y;
  rz              = z - body2->position.z;
  p.localPoint2.x = rx * tr[0] + ry * tr[3] + rz * tr[6];
  p.localPoint2.y = rx * tr[1] + ry * tr[4] + rz * tr[7];
  p.localPoint2.z = rx * tr[2] + ry * tr[5] + rz * tr[8];

  p.normalImpulse = 0;
  if (flip) {
    p.normal.x = -normalX;
    p.normal.y = -normalY;
    p.normal.z = -normalZ;
  }
  else {
    p.normal.x = normalX;
    p.normal.y = normalY;
    p.normal.z = normalZ;
  }
  p.penetration = penetration;
  p.warmStarted = false;
}

} // end of namespace OIMO
