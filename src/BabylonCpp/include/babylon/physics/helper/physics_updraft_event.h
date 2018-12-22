#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>
#include <babylon/physics/helper/physics_helper_enums.h>

namespace BABYLON {

class EventState;
struct IPhysicsEngine;
class Mesh;
struct PhysicsForceAndContactPoint;
class PhysicsImpostor;
struct PhysicsUpdraftEventData;
class Scene;
using IPhysicsEnginePtr = std::shared_ptr<IPhysicsEngine>;
using MeshPtr           = std::shared_ptr<Mesh>;

/**
 * @brief Represents a physics updraft event.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsUpdraftEvent {

public:
  /**
   * @brief Initializes the physics updraft event.
   * @param _scene BabylonJS scene
   * @param _origin The origin position of the updraft
   * @param _radius The radius of the updraft
   * @param _strength The strength of the updraft
   * @param _height The height of the updraft
   * @param _updraftMode The mode of the updraft
   */
  PhysicsUpdraftEvent(Scene* scene, Vector3 origin, float radius,
                      float strength, float height,
                      PhysicsUpdraftMode updraftMode);
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
   * @brief Disposes the sphere.
   * @param force Specifies if the updraft should be disposed by force
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
  PhysicsUpdraftMode _updraftMode;
  IPhysicsEnginePtr _physicsEngine;
  // the most upper part of the cylinder
  Vector3 _originTop;
  // used if the updraftMode is perpendicular
  Vector3 _originDirection;
  float _updraftMultiplier;
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
