#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_EVENT_OPTIONS_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_EVENT_OPTIONS_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/physics/helper/physics_helper_enums.h>

namespace BABYLON {

struct PhysicsAffectedImpostorWithData;
class SphereOptions;

//------------------------------------------------------------------------------
// Physics radial explosion event options
//------------------------------------------------------------------------------

/**
 * @brief Options for the radial explosion event.
 * @see
 * https://doc.babylonjs.com/how_to/using_the_physics_engine#further-functionality-of-the-impostor-class
 */
class BABYLON_SHARED_EXPORT PhysicsRadialExplosionEventOptions {

public:
  PhysicsRadialExplosionEventOptions();
  PhysicsRadialExplosionEventOptions(
    const PhysicsRadialExplosionEventOptions& other);
  PhysicsRadialExplosionEventOptions(
    PhysicsRadialExplosionEventOptions&& other);
  PhysicsRadialExplosionEventOptions&
  operator=(const PhysicsRadialExplosionEventOptions& other);
  PhysicsRadialExplosionEventOptions&
  operator=(PhysicsRadialExplosionEventOptions&& other);
  ~PhysicsRadialExplosionEventOptions();

public:
  /**
   * The radius of the sphere for the radial explosion.
   */
  float radius;

  /**
   * The strenth of the explosion.
   */
  float strength;

  /**
   * The strenght of the force in correspondence to the distance of the affected
   * object
   */
  PhysicsRadialImpulseFalloff falloff;

  /**
   * Sphere options for the radial explosion.
   */
  SphereOptions sphere;

  /**
   * Sphere options for the radial explosion.
   */
  std::function<void(const std::vector<PhysicsAffectedImpostorWithData>&
                       affectedImpostorsWithData)>
    affectedImpostorsCallback;

}; // end of class PhysicsRadialExplosionEventOptions

//------------------------------------------------------------------------------
// Physics updraft event options
//------------------------------------------------------------------------------

/**
 * @brief Options for the updraft event.
 * @see
 * https://doc.babylonjs.com/how_to/using_the_physics_engine#further-functionality-of-the-impostor-class
 */
struct BABYLON_SHARED_EXPORT PhysicsUpdraftEventOptions {
  /**
   * The radius of the cylinder for the vortex
   */
  float radius = 5.f;

  /**
   * The strenth of the updraft.
   */
  float strength = 10.f;

  /**
   * The height of the cylinder for the updraft.
   */
  float height = 10.f;

  /**
   * The mode for the the updraft.
   */
  PhysicsUpdraftMode updraftMode = PhysicsUpdraftMode::Center;
}; // end of struct PhysicsUpdraftEventOptions

//------------------------------------------------------------------------------
// Physics vortex event options
//------------------------------------------------------------------------------

/**
 * @brief Options for the vortex event.
 * @see
 * https://doc.babylonjs.com/how_to/using_the_physics_engine#further-functionality-of-the-impostor-class
 */
struct BABYLON_SHARED_EXPORT PhysicsVortexEventOptions {
  /**
   * The radius of the cylinder for the vortex
   */
  float radius = 5.f;

  /**
   * The strenth of the vortex.
   */
  float strength = 10.f;

  /**
   * The height of the cylinder for the vortex.
   */
  float height = 10.f;

  /**
   * At which distance, relative to the radius the centripetal forces should
   * kick in? Range: 0-1
   */
  float centripetalForceThreshold = 0.7f;

  /**
   * This multiplier determines with how much force the objects will be pushed
   * sideways/around the vortex, when below the treshold.
   */
  float centripetalForceMultiplier = 5.f;

  /**
   * This multiplier determines with how much force the objects will be pushed
   * sideways/around the vortex, when above the treshold.
   */
  float centrifugalForceMultiplier = 0.5f;

  /**
   * This multiplier determines with how much force the objects will be pushed
   * upwards, when in the vortex.
   */
  float updraftForceMultiplier = 0.02f;
}; // end of struct PhysicsVortexEventOptions

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_EVENT_OPTIONS_H
