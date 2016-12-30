#ifndef OIMO_COLLISION_NARROWPHASE_BOX_CYLINDER_COLLISION_DETECTOR_H
#define OIMO_COLLISION_NARROWPHASE_BOX_CYLINDER_COLLISION_DETECTOR_H

#include <oimo/collision/narrowphase/collision_detector.h>

namespace OIMO {

class BoxShape;
class ContactManifold;
class CylinderShape;
class Shape;
class Vec3;

/**
 * @brief A collision detector which detects collisions between box and
 * cylinder.
 */
class BoxCylinderCollisionDetector : public CollisionDetector {

public:
  BoxCylinderCollisionDetector(bool flip);
  ~BoxCylinderCollisionDetector();

  bool getSep(BoxShape* b, CylinderShape* c, Vec3& sep, Vec3& pos, Vec3& dep);
  void supportPointB(BoxShape* b, float dx, float dy, float dz, Vec3& out);
  void supportPointC(CylinderShape* c, float dx, float dy, float dz, Vec3& out);
  void detectCollision(Shape* shape1, Shape* shape2,
                       ContactManifold* manifold) override;

}; // end of class BoxCylinderCollisionDetector

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_NARROWPHASE_BOX_CYLINDER_COLLISION_DETECTOR_H
