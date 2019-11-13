#include <babylon/physics/helper/physics_gravitational_field_event.h>

#include <babylon/engines/scene.h>
#include <babylon/meshes/mesh.h>
#include <babylon/physics/helper/physics_event_data.h>
#include <babylon/physics/helper/physics_helper.h>
#include <babylon/physics/helper/physics_radial_explosion_event.h>

namespace BABYLON {

PhysicsGravitationalFieldEvent::PhysicsGravitationalFieldEvent(
  PhysicsHelper* physicsHelper, Scene* scene, const Vector3& origin,
  const PhysicsRadialExplosionEventOptions& options)
    : _physicsHelper{physicsHelper}
    , _scene{scene}
    , _origin{origin}
    , _tickCallback{nullptr}
    , _sphere{nullptr}
    , _dataFetched{false}
{
  _tickCallback
    = [this](Scene* /*scene*/, EventState & /*es*/) -> void { _tick(); };

  PhysicsRadialExplosionEventOptions eventOptions;
  eventOptions.strength = options.strength * -1.f;
  _options              = eventOptions;
}

PhysicsGravitationalFieldEvent::~PhysicsGravitationalFieldEvent() = default;

PhysicsGravitationalFieldEventData PhysicsGravitationalFieldEvent::getData()
{
  _dataFetched = true;

  return {
    _sphere // sphere
  };
}

void PhysicsGravitationalFieldEvent::enable()
{
  _tick();
  _scene->registerBeforeRender(_tickCallback);
}

void PhysicsGravitationalFieldEvent::disable()
{
  _scene->unregisterBeforeRender(_tickCallback);
}

void PhysicsGravitationalFieldEvent::dispose(bool force)
{
  if (!_sphere) {
    return;
  }
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
    _physicsHelper->applyRadialExplosionForce(_origin, _options);
  }
  else {
    auto radialExplosionEvent
      = _physicsHelper->applyRadialExplosionForce(_origin, _options);
    if (radialExplosionEvent) {
      _sphere = radialExplosionEvent->getData().sphere->clone(
        "radialExplosionEventSphereClone");
    }
  }
}

} // end of namespace BABYLON
