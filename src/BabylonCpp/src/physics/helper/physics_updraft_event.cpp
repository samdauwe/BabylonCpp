#include <babylon/physics/helper/physics_updraft_event.h>

#include <babylon/engines/scene.h>
#include <babylon/meshes/builders/cylinder_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/physics/helper/physics_event_data.h>
#include <babylon/physics/helper/physics_hit_data.h>
#include <babylon/physics/iphysics_enabled_object.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {

PhysicsUpdraftEvent::PhysicsUpdraftEvent(
  Scene* scene, const Vector3& origin,
  const PhysicsUpdraftEventOptions& options)
    : _scene{scene}
    , _origin{origin}
    , _options{options}
    , _physicsEngine{nullptr}
    , _originTop{Vector3::Zero()}
    , _originDirection{Vector3::Zero()}
    , _tickCallback{nullptr}
    , _cylinder{nullptr}
    , _cylinderPosition{Vector3::Zero()}
    , _dataFetched{false}
{
  _physicsEngine = _scene->getPhysicsEngine();

  _origin.addToRef(Vector3(0.f, _options.height / 2.f, 0.f), _cylinderPosition);
  _origin.addToRef(Vector3(0.f, _options.height, 0.f), _originTop);

  if (_options.updraftMode == PhysicsUpdraftMode::Perpendicular) {
    _originDirection = _origin.subtract(_originTop).normalize();
  }

  _tickCallback
    = [this](Scene* /*scene*/, EventState & /*es*/) -> void { _tick(); };

  _prepareCylinder();
}

PhysicsUpdraftEvent::~PhysicsUpdraftEvent() = default;

PhysicsUpdraftEventData PhysicsUpdraftEvent::getData()
{
  _dataFetched = true;

  return {
    _cylinder // cylinder
  };
}

void PhysicsUpdraftEvent::enable()
{
  _tick();
  _scene->registerBeforeRender(_tickCallback);
}

void PhysicsUpdraftEvent::disable()
{
  _scene->unregisterBeforeRender(_tickCallback);
}

void PhysicsUpdraftEvent::dispose(bool force)
{
  if (!_cylinder) {
    return;
  }
  if (force) {
    _cylinder->dispose();
  }
  else {
    if (!_dataFetched) {
      _cylinder->dispose();
    }
  }
}

std::unique_ptr<PhysicsHitData>
PhysicsUpdraftEvent::getImpostorHitData(PhysicsImpostor& impostor)
{
  if (impostor.mass == 0.f) {
    return nullptr;
  }

  if (!_intersectsWithCylinder(impostor)) {
    return nullptr;
  }

  auto impostorObjectCenter = impostor.getObjectCenter();

  Vector3 direction;
  if (_options.updraftMode == PhysicsUpdraftMode::Perpendicular) {
    direction = _originDirection;
  }
  else {
    direction = impostorObjectCenter.subtract(_originTop);
  }

  auto distanceFromOrigin = Vector3::Distance(_origin, impostorObjectCenter);

  auto multiplier = _options.strength * -1.f;

  auto force = direction.multiplyByFloats(multiplier, multiplier, multiplier);

  return std::make_unique<PhysicsHitData>(PhysicsHitData{
    force,                // force,
    impostorObjectCenter, // contactPoint
    distanceFromOrigin    // distanceFromOrigin
  });
}

void PhysicsUpdraftEvent::_tick()
{
  const auto& impostors = _physicsEngine->getImpostors();
  for (const auto& impostor : impostors) {
    auto impostorHitData = getImpostorHitData(*impostor);
    if (!impostorHitData) {
      return;
    }

    impostor->applyForce(impostorHitData->force, impostorHitData->contactPoint);
  }
}

void PhysicsUpdraftEvent::_prepareCylinder()
{
  if (!_cylinder) {
    CylinderOptions options;
    options.height   = _options.height;
    options.diameter = _options.radius * 2.f;
    _cylinder = CylinderBuilder::CreateCylinder("updraftEventCylinder", options,
                                                _scene);
    _cylinder->isVisible = false;
  }
}

bool PhysicsUpdraftEvent::_intersectsWithCylinder(PhysicsImpostor& impostor)
{
  auto impostorObject = static_cast<AbstractMesh*>(impostor.object);

  _cylinder->position = _cylinderPosition;

  return _cylinder->intersectsMesh(*impostorObject, true);
}

} // end of namespace BABYLON
