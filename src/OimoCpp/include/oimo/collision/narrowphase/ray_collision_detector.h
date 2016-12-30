#ifndef OIMO_COLLISION_NARROWPHASE_RAY_COLLISION_DETECTOR_H
#define OIMO_COLLISION_NARROWPHASE_RAY_COLLISION_DETECTOR_H

#include <array>

#include <oimo/collision/narrowphase/collision_detector.h>
#include <oimo/shape/face.h>

namespace OIMO {

class ContactManifold;
class Shape;
class Vec3;

/**
 * @brief Class for collision detection based on ray casting. This class should
 * only be used with the tetra or a polygon.
 */
class RayCollisionDetector : public CollisionDetector {

public:
  RayCollisionDetector();
  ~RayCollisionDetector();

  void detectCollision(Shape* shape1, Shape* shape2,
                       ContactManifold* manifold) override;

private:
  Vec3 _triangleIntersect(const Vec3& origin, const Vec3& direction,
                          const Face& face, bool backfaceCulling,
                          bool& triangleIntersect) const;

}; // end of class RayCollisionDetector

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_NARROWPHASE_RAY_COLLISION_DETECTOR_H
