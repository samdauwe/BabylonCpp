#ifndef OIMO_COLLISION_NARROWPHASE_SPHERE_CYLINDER_COLLISION_DETECTOR_H
#define OIMO_COLLISION_NARROWPHASE_SPHERE_CYLINDER_COLLISION_DETECTOR_H

#include <oimo/collision/narrowphase/collision_detector.h>

namespace OIMO {

class ContactManifold;
class Shape;

/**
 * @brief A collision detector which detects collisions between sphere and
 * cylinder.
 */
class SphereCylinderCollisionDetector : public CollisionDetector {

public:
  SphereCylinderCollisionDetector(bool flip);
  ~SphereCylinderCollisionDetector();

  void detectCollision(Shape* shape1, Shape* shape2,
                       ContactManifold* manifold) override;

}; // end of class SphereCylinderCollisionDetector

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_NARROWPHASE_SPHERE_CYLINDER_COLLISION_DETECTOR_H
