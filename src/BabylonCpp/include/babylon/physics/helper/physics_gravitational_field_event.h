#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_H

#include <functional>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>
#include <babylon/physics/helper/physics_event_options.h>

namespace BABYLON {

class EventState;
class Mesh;
struct PhysicsGravitationalFieldEventData;
class PhysicsHelper;
class Scene;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Represents a gravitational field event.
 */
class BABYLON_SHARED_EXPORT PhysicsGravitationalFieldEvent {

public:
  using RadiusOrPhysicsRadialExplosionEventOptions
    = std::variant<float, PhysicsRadialExplosionEventOptions>;

public:
  /**
   * @brief Initializes the physics gravitational field event.
   * @param physicsHelper A physics helper
   * @param scene BabylonJS scene
   * @param origin The origin position of the gravitational field event
   * @param options The options for the vortex event
   */
  PhysicsGravitationalFieldEvent(
    PhysicsHelper* physicsHelper, Scene* scene, const Vector3& origin,
    const PhysicsRadialExplosionEventOptions& options);
  ~PhysicsGravitationalFieldEvent();

  /**
   * @brief Returns the data related to the gravitational field event (sphere).
   * @returns A gravitational field event
   */
  PhysicsGravitationalFieldEventData getData();

  /**
   * @brief Enables the gravitational field.
   */
  void enable();

  /**
   * @brief Disables the gravitational field.
   */
  void disable();

  /**
   * @brief Disposes the sphere.
   * @param force The force to dispose from the gravitational field event
   */
  void dispose(bool force = true);

private:
  void _tick();

private:
  PhysicsHelper* _physicsHelper;
  Scene* _scene;
  Vector3 _origin;
  RadiusOrPhysicsRadialExplosionEventOptions _options;
  std::function<void(Scene* scene, EventState& es)> _tickCallback;
  MeshPtr _sphere;
  // check if the has been fetched the data. If not, do cleanup
  bool _dataFetched;

}; // end of class PhysicsGravitationalFieldEvent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_H
