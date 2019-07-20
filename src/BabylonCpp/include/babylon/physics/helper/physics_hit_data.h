#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_HIT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_HIT_DATA_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Interface for a physics hit data
 * @see
 * https://doc.babylonjs.com/how_to/using_the_physics_engine#further-functionality-of-the-impostor-class
 */
struct BABYLON_SHARED_EXPORT PhysicsHitData {
  /**
   * The force applied at the contact point
   */
  Vector3 force;
  /**
   * The contact point
   */
  Vector3 contactPoint;
  /**
   * The distance from the origin to the contact point
   */
  float distanceFromOrigin = 0.f;
}; // end of struct PhysicsHitData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_HIT_DATA_H
