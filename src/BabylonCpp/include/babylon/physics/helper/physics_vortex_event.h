#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class EventState;
struct IPhysicsEngine;
class Mesh;
struct PhysicsForceAndContactPoint;
class PhysicsImpostor;
struct PhysicsVortexEventData;
class Scene;
using IPhysicsEnginePtr = std::shared_ptr<IPhysicsEngine>;
using MeshPtr           = std::shared_ptr<Mesh>;

/**
 * @brief Represents a physics vortex event.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsVortexEvent {

public:
  /**
   * @brief Initializes the physics vortex event.
   * @param _scene The BabylonJS scene
   * @param _origin The origin position of the vortex
   * @param _radius The radius of the vortex
   * @param _strength The strength of the vortex
   * @param _height The height of the vortex
   */
  PhysicsVortexEvent(Scene* scene, Vector3 origin, float radius, float strength,
                     float height);
  ~PhysicsVortexEvent();

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
   * @brief Disables the cortex.
   */
  void disable();

  /**
   * @brief Disposes the sphere.
   * @param force
   */
  void dispose(bool force = true);

private:
  std::unique_ptr<PhysicsForceAndContactPoint>
  getImpostorForceAndContactPoint(PhysicsImpostor* impostor);

  void _tick();

  /*** Helpers ***/

  void _prepareCylinder();

  bool _intersectsWithCylinder(PhysicsImpostor* impostor);

private:
  Scene* _scene;
  Vector3 _origin;
  float _radius;
  float _strength;
  float _height;
  IPhysicsEnginePtr _physicsEngine;
  // the most upper part of the cylinder
  Vector3 _originTop;
  // at which distance, relative to the radius the centripetal forces should
  // kick in
  float _centripetalForceThreshold;
  float _updraftMultiplier;
  std::function<void(Scene* scene, EventState& es)> _tickCallback;
  MeshPtr _cylinder;
  // to keep the cylinders position, because normally the origin is in the
  // center and not on the bottom
  Vector3 _cylinderPosition;
  // check if the has been fetched the data. If not, do cleanup
  bool _dataFetched;

}; // end of class PhysicsVortexEvent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_H
