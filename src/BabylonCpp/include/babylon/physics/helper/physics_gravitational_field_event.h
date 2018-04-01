#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>
#include <babylon/physics/helper/physics_helper_enums.h>

namespace BABYLON {

/**
 * @brief Gravitational Field.
 */
class BABYLON_SHARED_EXPORT PhysicsGravitationalFieldEvent {

public:
  PhysicsGravitationalFieldEvent(PhysicsHelper* physicsHelper, Scene* scene,
                                 const Vector3& origin, float radius,
                                 float strength,
                                 PhysicsRadialImpulseFalloff falloff
                                 = PhysicsRadialImpulseFalloff::Constant);
  ~PhysicsGravitationalFieldEvent();

  /**
   * @brief Returns the data related to the gravitational field event (sphere).
   * @returns {PhysicsGravitationalFieldEventData}
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
   * @param {bolean} force
   */
  void dispose(bool force = true);

private:
  void _tick();

private:
  PhysicsHelper* _physicsHelper;
  Scene* _scene;
  Vector3 _origin;
  float _radius;
  float _strength;
  PhysicsRadialImpulseFalloff _falloff;
  ::std::function<void(Scene* scene, EventState& es)> _tickCallback;
  Mesh* _sphere;
  // check if the has been fetched the data. If not, do cleanup
  bool _dataFetched;

}; // end of class PhysicsGravitationalFieldEvent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_H
