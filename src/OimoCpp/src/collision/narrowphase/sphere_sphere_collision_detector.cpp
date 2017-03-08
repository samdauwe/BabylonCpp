#include <oimo/collision/narrowphase/sphere_sphere_collision_detector.h>

#include <cmath>

#include <oimo/collision/shape/shape.h>
#include <oimo/collision/shape/sphere_shape.h>
#include <oimo/constraint/contact/contact_manifold.h>

namespace OIMO {

SphereSphereCollisionDetector::SphereSphereCollisionDetector()
    : CollisionDetector{}
{
}

SphereSphereCollisionDetector::~SphereSphereCollisionDetector()
{
}

void SphereSphereCollisionDetector::detectCollision(Shape* shape1,
                                                    Shape* shape2,
                                                    ContactManifold* manifold)
{
  if (shape1->type != Shape::Type::SHAPE_SPHERE
      || shape2->type != Shape::Type::SHAPE_SPHERE) {
    return;
  }
  auto s1        = dynamic_cast<SphereShape*>(shape1);
  auto s2        = dynamic_cast<SphereShape*>(shape2);
  const auto& p1 = s1->position;
  const auto& p2 = s2->position;
  float dx       = p2.x - p1.x;
  float dy       = p2.y - p1.y;
  float dz       = p2.z - p1.z;
  float len      = dx * dx + dy * dy + dz * dz;
  float r1       = s1->radius;
  float r2       = s2->radius;
  float rad      = r1 + r2;
  if (len > 0.f && len < rad * rad) {
    len          = std::sqrt(len);
    float invLen = 1.f / len;
    dx *= invLen;
    dy *= invLen;
    dz *= invLen;
    manifold->addPoint(p1.x + dx * r1, p1.y + dy * r1, p1.z + dz * r1, dx, dy,
                       dz, len - rad, false);
  }
}

} // end of namespace OIMO
