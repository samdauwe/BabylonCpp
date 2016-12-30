#ifndef OIMO_COLLISION_NARROWPHASE_TETRA_TETRA_COLLISION_DETECTOR_H
#define OIMO_COLLISION_NARROWPHASE_TETRA_TETRA_COLLISION_DETECTOR_H

#include <oimo/collision/narrowphase/collision_detector.h>

namespace OIMO {

class ContactManifold;
class Shape;
class Vec3;

/**
 * @brief Class for checking collisions between 2 tetras, a shape that is made
 * with 4 vertices and 4 faces arranged in triangles. With this algorigthm, soft
 * body physics are possible and easier to implement.
 */
class TetraTetraCollisionDetector : public CollisionDetector {

public:
  TetraTetraCollisionDetector();
  ~TetraTetraCollisionDetector();

  void detectCollision(Shape* shape1, Shape* shape2,
                       ContactManifold* manifold) override;

private:
  bool _tricheck(const Vec3& p, const Vec3& p0, const Vec3& p1,
                 const Vec3& p2) const;
  Vec3 _pt(float x, float y) const;

}; // end of class TetraTetraCollisionDetector

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_NARROWPHASE_TETRA_TETRA_COLLISION_DETECTOR_H
