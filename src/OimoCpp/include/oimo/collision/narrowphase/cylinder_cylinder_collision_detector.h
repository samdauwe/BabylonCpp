#ifndef OIMO_COLLISION_NARROWPHASE_CYLINDER_CYLINDER_COLLISION_DETECTOR_H
#define OIMO_COLLISION_NARROWPHASE_CYLINDER_CYLINDER_COLLISION_DETECTOR_H

#include <oimo/collision/narrowphase/collision_detector.h>

namespace OIMO {

class ContactManifold;
class CylinderShape;
class Shape;
class Vec3;

/**
 * @brief A collision detector which detects collisions between two cylinders.
 */
class CylinderCylinderCollisionDetector : public CollisionDetector {

public:
  CylinderCylinderCollisionDetector();
  ~CylinderCylinderCollisionDetector();

  bool getSep(CylinderShape* c1, CylinderShape* c2, Vec3& sep, Vec3& pos,
              Vec3& out);
  void supportPoint(CylinderShape* c, float dx, float dy, float dz, Vec3& dep);
  void detectCollision(Shape* shape1, Shape* shape2,
                       ContactManifold* manifold) override;

}; // end of class CylinderCylinderCollisionDetector

} // end of namespace OIMO

#endif // end of
       // OIMO_COLLISION_NARROWPHASE_CYLINDER_CYLINDER_COLLISION_DETECTOR_H
