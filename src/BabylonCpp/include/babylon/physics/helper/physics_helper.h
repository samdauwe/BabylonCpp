#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/physics/helper/physics_helper_enums.h>

namespace BABYLON {

struct IPhysicsEngine;
class PhysicsGravitationalFieldEvent;
class PhysicsRadialExplosionEvent;
class PhysicsUpdraftEvent;
class PhysicsVortexEvent;
class Scene;
class Vector3;
using IPhysicsEnginePtr = std::shared_ptr<IPhysicsEngine>;

/**
 * @brief A helper for physics simulations.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsHelper {

public:
  /**
   * @brief Initializes the Physics helper.
   * @param scene Babylon.js scene
   */
  PhysicsHelper(Scene* scene);
  ~PhysicsHelper();

  /**
   * @brief Applies a radial explosion impulse.
   * @param origin the origin of the explosion
   * @param radius the explosion radius
   * @param strength the explosion strength
   * @param falloff possible options: Constant & Linear. Defaults to Constant
   * @returns A physics radial explosion event, or null
   */
  std::unique_ptr<PhysicsRadialExplosionEvent>
  applyRadialExplosionImpulse(const Vector3& origin, float radius,
                              float strength,
                              PhysicsRadialImpulseFalloff falloff
                              = PhysicsRadialImpulseFalloff::Constant);

  /**
   * @brief Applies a radial explosion force.
   * @param origin the origin of the explosion
   * @param radius the explosion radius
   * @param strength the explosion strength
   * @param falloff possible options: Constant & Linear. Defaults to Constant
   * @returns A physics radial explosion event, or null
   */
  std::unique_ptr<PhysicsRadialExplosionEvent>
  applyRadialExplosionForce(const Vector3& origin, float radius, float strength,
                            PhysicsRadialImpulseFalloff falloff
                            = PhysicsRadialImpulseFalloff::Constant);

  /**
   * @brief Creates a gravitational field.
   * @param origin the origin of the explosion
   * @param radius the explosion radius
   * @param strength the explosion strength
   * @param falloff possible options: Constant & Linear. Defaults to Constant
   * @returns A physics gravitational field event, or null
   */
  std::unique_ptr<PhysicsGravitationalFieldEvent>
  gravitationalField(const Vector3& origin, float radius, float strength,
                     PhysicsRadialImpulseFalloff falloff
                     = PhysicsRadialImpulseFalloff::Constant);

  /**
   * @brief Creates a physics updraft event.
   * @param origin the origin of the updraft
   * @param radius the radius of the updraft
   * @param strength the strength of the updraft
   * @param height the height of the updraft
   * @param updraftMode possible options: Center & Perpendicular. Defaults to
   * Center
   * @returns A physics updraft event, or null
   */
  std::unique_ptr<PhysicsUpdraftEvent>
  updraft(const Vector3& origin, float radius, float strength, float height,
          PhysicsUpdraftMode updraftMode = PhysicsUpdraftMode::Center);

  /**
   * @brief Creates a physics vortex event.
   * @param origin the of the vortex
   * @param radius the radius of the vortex
   * @param strength the strength of the vortex
   * @param height   the height of the vortex
   * @returns a Physics vortex event, or null
   * A physics vortex event or null
   */
  std::unique_ptr<PhysicsVortexEvent>
  vortex(const Vector3& origin, float radius, float strength, float height);

private:
  Scene* _scene;
  IPhysicsEnginePtr _physicsEngine;

}; // end of class PhysicsHelper

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_H
