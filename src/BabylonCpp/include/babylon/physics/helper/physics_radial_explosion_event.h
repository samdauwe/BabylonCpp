#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/physics/helper/physics_event_options.h>

namespace BABYLON {

struct PhysicsForceAndContactPoint;
class PhysicsImpostor;
struct PhysicsRadialExplosionEventData;
class Ray;
class Scene;
FWD_CLASS_SPTR(Mesh)
FWD_STRUCT_SPTR(PhysicsHitData)

/**
 * @brief Represents a physics radial explosion event
 */
class BABYLON_SHARED_EXPORT PhysicsRadialExplosionEvent {

public:
  /**
   * @brief Initializes a radial explosion event.
   * @param _scene BabylonJS scene
   * @param _options The options for the vortex event
   */
  PhysicsRadialExplosionEvent(Scene* scene, const PhysicsRadialExplosionEventOptions& options);
  ~PhysicsRadialExplosionEvent(); // = default

  /**
   * @brief Returns the data related to the radial explosion event (sphere).
   * @returns The radial explosion event data
   */
  PhysicsRadialExplosionEventData getData();

  /**
   * @brief Returns the force and contact point of the impostor or false, if the impostor is not
   * affected by the force/impulse.
   * @param impostor A physics imposter
   * @param origin the origin of the explosion
   * @returns {Nullable<PhysicsHitData>} A physics force and contact point, or null
   */
  PhysicsHitDataPtr getImpostorHitData(PhysicsImpostor& impostor, const Vector3& origin);

  /**
   * @brief Triggers affected impostors callbacks.
   * @param affectedImpostorsWithData defines the list of affected impostors (including associated
   * data)
   */
  void triggerAffectedImpostorsCallback(
    const std::vector<PhysicsAffectedImpostorWithData>& affectedImpostorsWithData);

  /**
   * @brief Disposes the sphere.
   * @param force Specifies if the sphere should be disposed by force
   */
  void dispose(bool force = true);

private:
  /*** Helpers ***/

  void _prepareSphere();

  bool _intersectsWithSphere(PhysicsImpostor& impostor, const Vector3& origin, float radius);

private:
  Scene* _scene;
  PhysicsRadialExplosionEventOptions _options;
  // create a sphere, so we can get the intersecting meshes inside
  MeshPtr _sphere;
  // check if the data has been fetched. If not, do cleanup
  bool _dataFetched;

}; // end of class PhysicsRadialExplosionEvent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_H
