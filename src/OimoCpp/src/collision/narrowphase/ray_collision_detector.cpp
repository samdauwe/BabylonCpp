#include <oimo/collision/narrowphase/ray_collision_detector.h>

#include <oimo/collision/shape/tetra_shape.h>
#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/math/vec3.h>

namespace OIMO {

RayCollisionDetector::RayCollisionDetector() : CollisionDetector{}
{
}

RayCollisionDetector::~RayCollisionDetector()
{
}

void RayCollisionDetector::detectCollision(Shape* shape1, Shape* shape2,
                                           ContactManifold* manifold)
{
  if (shape2->type != Shape::Type::SHAPE_TETRA) {
    return;
  }
  auto _shape2 = dynamic_cast<TetraShape*>(shape2);

  const auto& pos1 = shape1->position;
  const auto& pos2 = shape2->position;
  Vec3 vec3_1(pos1.x, pos1.y, pos1.z);
  Vec3 vec3_2(pos2.x, pos2.y, pos2.z);
  Vec3 intersect;
  bool triangleIntersect = false;

  // Yes, it is a brute force approach but it works for now...
  for (size_t i = 0; i < _shape2->faces.size(); ++i) {
    intersect = _triangleIntersect(vec3_1, vec3_1.angleTo(vec3_2),
                                   _shape2->faces[i], false, triangleIntersect);

    if (triangleIntersect) {
      manifold->addPoint(intersect.x, intersect.y, intersect.z);
    }
  }
}

Vec3 RayCollisionDetector::_triangleIntersect(const Vec3& origin,
                                              const Vec3& direction,
                                              const Face& face,
                                              bool backfaceCulling,
                                              bool& triangleIntersect) const
{
  Vec3 diff, edge1, edge2, normal, result;

  Vertex _a = static_cast<float>(face.a), _b = static_cast<float>(face.b),
         _c = static_cast<float>(face.c);
  Vec3 a(_a.x, _a.y, _a.z);
  Vec3 b(_b.x, _b.y, _b.z);
  Vec3 c(_c.x, _c.y, _c.z);
  float sign, DdN;

  edge1.subVectors(b, a);
  edge2.subVectors(c, a);
  normal.crossVectors(edge1, edge2);

  DdN = direction.dot(normal);
  if (DdN > 0.f) {
    if (backfaceCulling) {
      triangleIntersect = false;
      return result;
    }
    sign = 1.f;
  }
  else if (DdN < 0.f) {
    sign = -1.f;
    DdN  = -DdN;
  }
  else {
    triangleIntersect = false;
    return result;
  }

  diff.subVectors(origin, a);
  float DdQxE2 = sign * direction.dot(edge2.crossVectors(diff, edge2));

  // b1 < 0, no intersection
  if (DdQxE2 < 0.f) {
    triangleIntersect = false;
    return result;
  }

  float DdE1xQ = sign * direction.dot(edge1.cross(diff));

  // b2 < 0, no intersection
  if (DdE1xQ < 0.f) {
    triangleIntersect = false;
    return result;
  }

  // b1+b2 > 1, no intersection
  if (DdQxE2 + DdE1xQ > DdN) {
    triangleIntersect = false;
    return result;
  }

  // Line intersects triangle, check if ray does.
  float QdN = -sign * diff.dot(normal);

  // t < 0, no intersection
  if (QdN < 0.f) {
    triangleIntersect = false;
    return result;
  }

  // Ray intersects triangle.
  triangleIntersect = true;
  return Vec3().copy(direction).multiplyScalar(QdN / DdN).add(origin);
}

} // end of namespace OIMO
