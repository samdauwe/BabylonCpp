#ifndef OIMO_COLLISION_NARROWPHASE_COLLISION_DETECTOR_H
#define OIMO_COLLISION_NARROWPHASE_COLLISION_DETECTOR_H

#include <oimo/collision/narrowphase/collision_detector.h>

namespace OIMO {

class ContactManifold;
class Shape;

/**
 * @brief A collision detector detects collisions between two shapes.
 */
class CollisionDetector {

public:
  CollisionDetector();
  virtual ~CollisionDetector();

  /**
   * Detect collision between two shapes.
   * @param shape1
   * @param shape2
   * @param manifold
   */
  virtual void detectCollision(Shape* shape1, Shape* shape2,
                               ContactManifold* manifold)
    = 0;

public:
  // Whether the collision detector flips two shapes in detectCollision or not.
  bool flip;

}; // end of class CollisionDetector

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_NARROWPHASE_COLLISION_DETECTOR_H
