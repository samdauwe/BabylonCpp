#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/physics/helper/physics_helper_enums.h>

namespace BABYLON {

struct PhysicsForceAndContactPoint;
class PhysicsImpostor;
struct PhysicsRadialExplosionEventData;
class Ray;
class Scene;

/**
 * @brief Radial explosion.
 */
class BABYLON_SHARED_EXPORT PhysicsRadialExplosionEvent {

public:
  PhysicsRadialExplosionEvent(Scene* scene);
  ~PhysicsRadialExplosionEvent();

  /**
   * @brief Returns the data related to the radial explosion event (sphere &
   * rays).
   * @returns {PhysicsRadialExplosionEventData}
   */
  PhysicsRadialExplosionEventData getData();

  /**
   * @brief Returns the force and contact point of the impostor or false, if the
   * impostor is not affected by the force/impulse.
   * @param impostor
   * @param {Vector3} origin the origin of the explosion
   * @param {number} radius the explosion radius
   * @param {number} strength the explosion strength
   * @param {PhysicsRadialImpulseFalloff} falloff possible options: Constant &
   * Linear
   * @returns {Nullable<PhysicsForceAndContactPoint>}
   */
  std::unique_ptr<PhysicsForceAndContactPoint> getImpostorForceAndContactPoint(
    PhysicsImpostor* impostor, const Vector3& origin, float radius,
    float strength, PhysicsRadialImpulseFalloff falloff);

  /**
   * @brief Disposes the sphere.
   * @param {bolean} force
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
