#include <babylon/physics/helper/physics_gravitational_field_event.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>
#include <babylon/physics/helper/physics_gravitational_field_event_data.h>
#include <babylon/physics/helper/physics_helper.h>
#include <babylon/physics/helper/physics_radial_explosion_event.h>
#include <babylon/physics/helper/physics_radial_explosion_event_data.h>

namespace BABYLON {

PhysicsGravitationalFieldEvent::PhysicsGravitationalFieldEvent(
  PhysicsHelper* physicsHelper, Scene* scene, const Vector3& origin,
  float radius, float strength, PhysicsRadialImpulseFalloff falloff)
    : _physicsHelper{physicsHelper}
    , _scene{scene}
    , _origin{origin}
    , _radius{radius}
    , _strength{strength}
    , _falloff{falloff}
    , _tickCallback{nullptr}
    , _sphere{nullptr}
    , _dataFetched{false}
{
}

PhysicsGravitationalFieldEvent::~PhysicsGravitationalFieldEvent()
{
}

PhysicsGravitationalFieldEventData PhysicsGravitationalFieldEvent::getData()
{
  _dataFetched = true;

  return {
    _sphere // sphere
  };
}

void PhysicsGravitationalFieldEvent::enable()
{
}

void PhysicsGravitationalFieldEvent::disable()
{
}

void PhysicsGravitationalFieldEvent::dispose(bool force)
{
  if (force) {
    _sphere->dispose();
  }
  else {
    if (!_dataFetched) {
      _sphere->dispose();
    }
  }
}

void PhysicsGravitationalFieldEvent::_tick()
{
  // Since the params won't change, we fetch the event only once
  if (_sphere) {
    _physicsHelper->applyRadialExplosionForce(_origin, _radius,
                                              _strength * -1.f, _falloff);
  }
  else {
    auto radialExplosionEvent = _physicsHelper->applyRadialExplosionForce(
      _origin, _radius, _strength * -1.f, _falloff);
    if (radialExplosionEvent) {
      _sphere = radialExplosionEvent->getData().sphere->clone(
        "radialExplosionEventSphereClone");
    }
  }
}

} // end of namespace BABYLON
