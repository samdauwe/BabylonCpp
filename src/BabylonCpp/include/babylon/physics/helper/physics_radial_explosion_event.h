#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/vertex_data_options.h>
#include <babylon/physics/helper/physics_helper_enums.h>

namespace BABYLON {

struct PhysicsForceAndContactPoint;
class PhysicsImpostor;
struct PhysicsRadialExplosionEventData;
class Ray;
class Scene;

/**
 * @brief Represents a physics radial explosion event.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsRadialExplosionEvent {

public:
  /**
   * @brief Initializes a radial explosioin event.
   * @param scene BabylonJS scene
   */
  PhysicsRadialExplosionEvent(Scene* scene);
  ~PhysicsRadialExplosionEvent();

  /**
   * @brief Returns the data related to the radial explosion event (sphere &
   * rays).
   * @returns The radial explosion event data
   */
  PhysicsRadialExplosionEventData getData();

  /**
   * @brief Returns the force and contact point of the impostor or false, if the
   * impostor is not affected by the force/impulse.
   * @param impostor A physics imposter
   * @param origin the origin of the explosion
   * @param radius the explosion radius
   * @param strength the explosion strength
   * @param falloff possible options: Constant & Linear
   * @returns {Nullable<PhysicsForceAndContactPoint>} A physics force and
   * contact point, or null
   */
  std::unique_ptr<PhysicsForceAndContactPoint> getImpostorForceAndContactPoint(
    PhysicsImpostor* impostor, const Vector3& origin, float radius,
    float strength, PhysicsRadialImpulseFalloff falloff);

  /**
   * @brief Disposes the sphere.
   * @param force Specifies if the sphere should be disposed by force
   */
  void dispose(bool force = true);

private:
  /*** Helpers ***/

  void _prepareSphere();

  bool _intersectsWithSphere(PhysicsImpostor* impostor, const Vector3& origin,
                             float radius);

private:
  Scene* _scene;
  // create a sphere, so we can get the intersecting meshes inside
  MeshPtr _sphere;
  SphereOptions _sphereOptions;
  std::vector<Ray> _rays;
  // check if the data has been fetched. If not, do cleanup
  bool _dataFetched;

}; // end of class PhysicsRadialExplosionEvent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_H
