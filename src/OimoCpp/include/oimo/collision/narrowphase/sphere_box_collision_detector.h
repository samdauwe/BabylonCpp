#ifndef OIMO_COLLISION_NARROWPHASE_SPHERE_BOX_COLLISION_DETECTOR_H
#define OIMO_COLLISION_NARROWPHASE_SPHERE_BOX_COLLISION_DETECTOR_H

#include <oimo/collision/narrowphase/collision_detector.h>

namespace OIMO {

class ContactManifold;
class Shape;

/**
 * @brief A collision detector which detects collisions between sphere and box.
 */
class SphereBoxCollisionDetector : public CollisionDetector {

public:
  SphereBoxCollisionDetector(bool flip);
  ~SphereBoxCollisionDetector();

  void detectCollision(Shape* shape1, Shape* shape2,
                       ContactManifold* manifold) override;

}; // end of class SphereBoxCollisionDetector

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_NARROWPHASE_SPHERE_BOX_COLLISION_DETECTOR_H
