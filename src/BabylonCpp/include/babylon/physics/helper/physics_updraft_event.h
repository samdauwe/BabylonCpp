#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>
#include <babylon/physics/helper/physics_helper_enums.h>

namespace BABYLON {

/**
 * @brief Updraft.
 */
class BABYLON_SHARED_EXPORT PhysicsUpdraftEvent {

public:
  PhysicsUpdraftEvent(Scene* scene, Vector3 origin, float radius,
                      float strength, float height,
                      PhysicsUpdraftMode updraftMode);
  ~PhysicsUpdraftEvent();

  /**
   * @brief Returns the data related to the updraft event (cylinder).
   * @returns {PhysicsUpdraftEventData}
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
   * @param {bolean} force
   */
  void dispose(bool force = true);

private:
  unique_ptr_t<PhysicsForceAndContactPoint>
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
  PhysicsEngine* _physicsEngine;
  // the most upper part of the cylinder
  Vector3 _originTop;
  // used if the updraftMode is perpendicular
  Vector3 _originDirection;
  float _updraftMultiplier;
  ::std::function<void(Scene* scene, EventState& es)> _tickCallback;
  Mesh* _cylinder;
  // to keep the cylinders position, because normally the origin is in the
  // center and not on the bottom
  Vector3 _cylinderPosition;
  // check if the has been fetched the data. If not, do cleanup
  bool _dataFetched;

}; // end of class PhysicsUpdraftEvent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_H
