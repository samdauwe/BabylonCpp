#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_FORCE_AND_CONTACT_POINT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_FORCE_AND_CONTACT_POINT_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Interface for a physics force and contact point.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT PhysicsForceAndContactPoint {
  /**
   * The force applied at the contact point
   */
  Vector3 force;
  /**
   * The contact point
   */
  Vector3 contactPoint;
}; // end of struct PhysicsForceAndContactPoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_FORCE_AND_CONTACT_POINT_H
