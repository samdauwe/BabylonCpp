#ifndef OIMO_COLLISION_NARROWPHASE_BOX_BOX_COLLISION_DETECTOR_H
#define OIMO_COLLISION_NARROWPHASE_BOX_BOX_COLLISION_DETECTOR_H

#include <array>

#include <oimo/collision/narrowphase/collision_detector.h>

namespace OIMO {

class ContactManifold;
class Shape;

/**
 * @brief A collision detector which detects collisions between two boxes.
 */
class BoxBoxCollisionDetector : public CollisionDetector {

public:
  BoxBoxCollisionDetector();
  ~BoxBoxCollisionDetector();

  void detectCollision(Shape* shape1, Shape* shape2,
                       ContactManifold* manifold) override;

private:
  // 8 vertices x,y,z
  std::array<float, 24> _clipVertices1;
  std::array<float, 24> _clipVertices2;
  std::array<bool, 8> _used;
  float _inf;

}; // end of class BoxBoxCollisionDetector

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_NARROWPHASE_BOX_BOX_COLLISION_DETECTOR_H
