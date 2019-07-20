#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>
#include <babylon/physics/helper/physics_event_options.h>

namespace BABYLON {

class EventState;
struct IPhysicsEngine;
class Mesh;
struct PhysicsHitData;
class PhysicsImpostor;
struct PhysicsUpdraftEventData;
class Scene;
using IPhysicsEnginePtr = std::shared_ptr<IPhysicsEngine>;
using MeshPtr           = std::shared_ptr<Mesh>;

/**
 * @brief Represents a physics updraft event.
 */
class BABYLON_SHARED_EXPORT PhysicsUpdraftEvent {

public:
  /**
   * @brief Initializes the physics updraft event.
   * @param scene BabylonJS scene
   * @param origin The origin position of the updraft
   * @param options The options for the updraft event
   */
  PhysicsUpdraftEvent(Scene* scene, const Vector3& origin,
                      const PhysicsUpdraftEventOptions& options);
  ~PhysicsUpdraftEvent();

  /**
   * @brief Returns the data related to the updraft event (cylinder).
   * @returns A physics updraft event
   */
  PhysicsUpdraftEventData getData();

  /**
   * @brief Enables the updraft.
   */
  void enable();

  /**
   * @brief Disables the updraft.
   */
  void disable();

  /**
   * @brief Disposes the cylinder.
   * @param force Specifies if the updraft should be disposed by force
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
  PhysicsUpdraftEventOptions _options;
  IPhysicsEnginePtr _physicsEngine;
  // the most upper part of the cylinder
  Vector3 _originTop;
  // used if the updraftMode is perpendicular
  Vector3 _originDirection;
  std::function<void(Scene* scene, EventState& es)> _tickCallback;
  MeshPtr _cylinder;
  // to keep the cylinders position, because normally the origin is in the
  // center and not on the bottom
  Vector3 _cylinderPosition;
  // check if the has been fetched the data. If not, do cleanup
  bool _dataFetched;

}; // end of class PhysicsUpdraftEvent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_H
