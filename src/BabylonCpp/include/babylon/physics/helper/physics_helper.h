#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_H

#include <babylon/babylon_global.h>
#include <babylon/physics/helper/physics_helper_enums.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PhysicsHelper {

public:
  PhysicsHelper(Scene* scene);
  ~PhysicsHelper();

  /**
   * @param {Vector3} origin the origin of the explosion
   * @param {number} radius the explosion radius
   * @param {number} strength the explosion strength
   * @param {PhysicsRadialImpulseFalloff} falloff possible options: Constant &
   * Linear. Defaults to Constant
   */
  unique_ptr_t<PhysicsRadialExplosionEvent>
  applyRadialExplosionImpulse(const Vector3& origin, float radius,
                              float strength,
                              PhysicsRadialImpulseFalloff falloff
                              = PhysicsRadialImpulseFalloff::Constant);

  /**
   * @param {Vector3} origin the origin of the explosion
   * @param {number} radius the explosion radius
   * @param {number} strength the explosion strength
   * @param {PhysicsRadialImpulseFalloff} falloff possible options: Constant &
   * Linear. Defaults to Constant
   */
  unique_ptr_t<PhysicsRadialExplosionEvent>
  applyRadialExplosionForce(const Vector3& origin, float radius, float strength,
                            PhysicsRadialImpulseFalloff falloff
                            = PhysicsRadialImpulseFalloff::Constant);

  /**
   * @param {Vector3} origin the origin of the explosion
   * @param {number} radius the explosion radius
   * @param {number} strength the explosion strength
   * @param {PhysicsRadialImpulseFalloff} falloff possible options: Constant &
   * Linear. Defaults to Constant
   */
  unique_ptr_t<PhysicsGravitationalFieldEvent>
  gravitationalField(const Vector3& origin, float radius, float strength,
                     PhysicsRadialImpulseFalloff falloff
                     = PhysicsRadialImpulseFalloff::Constant);

  /**
   * @param {Vector3} origin the origin of the updraft
   * @param {number} radius the radius of the updraft
   * @param {number} strength the strength of the updraft
   * @param {number} height the height of the updraft
   * @param {PhysicsUpdraftMode} updraftMode possible options: Center &
   * Perpendicular. Defaults to Center
   */
  unique_ptr_t<PhysicsUpdraftEvent>
  updraft(const Vector3& origin, float radius, float strength, float height,
          PhysicsUpdraftMode updraftMode = PhysicsUpdraftMode::Center);

  /**
   * @param {Vector3} origin the of the vortex
   * @param {number} radius the radius of the vortex
   * @param {number} strength the strength of the vortex
   * @param {number} height   the height of the vortex
   */
  unique_ptr_t<PhysicsVortexEvent> vortex(const Vector3& origin, float radius,
                                          float strength, float height);

private:
  Scene* _scene;
  PhysicsEngine* _physicsEngine;

}; // end of class PhysicsHelper

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_H
