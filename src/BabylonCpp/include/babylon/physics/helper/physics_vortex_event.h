#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/maths/vector3.h>
#include <babylon/physics/helper/physics_event_options.h>

namespace BABYLON {

class EventState;
struct IPhysicsEngine;
class Mesh;
class PhysicsImpostor;
struct PhysicsHitData;
struct PhysicsVortexEventData;
class Scene;
using IPhysicsEnginePtr = std::shared_ptr<IPhysicsEngine>;
using MeshPtr           = std::shared_ptr<Mesh>;

/**
 * @brief Represents a physics vortex event.
 */
class BABYLON_SHARED_EXPORT PhysicsVortexEvent {

public:
  /**
   * @brief Initializes the physics vortex event.
   * @param scene The BabylonJS scene
   * @param origin The origin position of the vortex
   * @param options The options for the vortex event
   */
  PhysicsVortexEvent(Scene* scene, const Vector3& origin,
                     const PhysicsVortexEventOptions& options);
  ~PhysicsVortexEvent(); // = default

  /**
   * @brief Returns the data related to the vortex event (cylinder).
   * @returns The physics vortex event data
   */
  PhysicsVortexEventData getData();

  /**
   * @brief Enables the vortex.
   */
  void enable();

  /**
   * @brief Disables the vortex.
   */
  void disable();

  /**
   * @brief Disposes the sphere.
   * @param force
   */
  void dispose(bool force = true);

private:
  std::unique_ptr<PhysicsHitData> getImpostorHitData(PhysicsImpostor& impostor);

  void _tick();

  /*** Helpers ***/

  void _prepareCylinder();

  bool _intersectsWithCylinder(PhysicsImpostor& impostor);

private:
  Scene* _scene;
  Vector3 _origin;
  PhysicsVortexEventOptions _options;
  IPhysicsEnginePtr _physicsEngine;
  // the most upper part of the cylinder
  Vector3 _originTop;
  std::function<void(Scene* scene, EventState& es)> _tickCallback;
  MeshPtr _cylinder;
  Vector3 _cylinderPosition;
  // check if the has been fetched the data. If not, do cleanup
  bool _dataFetched;

}; // end of class PhysicsVortexEvent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_H
