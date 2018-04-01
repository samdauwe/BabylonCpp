#include <babylon/physics/helper/physics_updraft_event.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/physics/helper/physics_force_and_contact_point.h>
#include <babylon/physics/helper/physics_updraft_event_data.h>
#include <babylon/physics/iphysics_enabled_object.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {

PhysicsUpdraftEvent::PhysicsUpdraftEvent(Scene* scene, Vector3 origin,
                                         float radius, float strength,
                                         float height,
                                         PhysicsUpdraftMode updraftMode)
    : _scene{scene}
    , _origin{origin}
    , _radius{radius}
    , _strength{strength}
    , _height{height}
    , _updraftMode{updraftMode}
    , _originTop{Vector3::Zero()}
    , _originDirection{Vector3::Zero()}
    , _tickCallback{nullptr}
    , _cylinder{nullptr}
    , _cylinderPosition{Vector3::Zero()}
    , _dataFetched{false}
{
  _physicsEngine = _scene->getPhysicsEngine();

  _origin.addToRef(Vector3(0.f, _height / 2.f, 0.f), _cylinderPosition);
  _origin.addToRef(Vector3(0.f, _height, 0.f), _originTop);

  if (_updraftMode == PhysicsUpdraftMode::Perpendicular) {
    _originDirection = _origin.subtract(_originTop).normalize();
  }
}

PhysicsUpdraftEvent::~PhysicsUpdraftEvent()
{
}

PhysicsUpdraftEventData PhysicsUpdraftEvent::getData()
{
  _dataFetched = true;

  return {
    _cylinder // cylinder
  };
}

void PhysicsUpdraftEvent::enable()
{
}

void PhysicsUpdraftEvent::disable()
{
}

void PhysicsUpdraftEvent::dispose(bool force)
{
  if (force) {
    _cylinder->dispose();
  }
  else {
    if (!_dataFetched) {
      _cylinder->dispose();
    }
  }
}

unique_ptr_t<PhysicsForceAndContactPoint>
PhysicsUpdraftEvent::getImpostorForceAndContactPoint(PhysicsImpostor* impostor)
{
  if (impostor->mass == 0.f) {
    return nullptr;
  }

  if (!_intersectsWithCylinder(impostor)) {
    return nullptr;
  }

  auto impostorObjectCenter = impostor->getObjectCenter();

  Vector3 direction;
  if (_updraftMode == PhysicsUpdraftMode::Perpendicular) {
    direction = _originDirection;
  }
  else {
    direction = impostorObjectCenter.subtract(_originTop);
  }

  auto multiplier = _strength * -1.f;

  auto force = direction.multiplyByFloats(multiplier, multiplier, multiplier);

  return ::std::make_unique<PhysicsForceAndContactPoint>(
    PhysicsForceAndContactPoint{
      force,               // force,
      impostorObjectCenter // contactPoint
    });
}

void PhysicsUpdraftEvent::_tick()
{
  const auto& impostors = _physicsEngine->getImpostors();
  for (const auto& impostor : impostors) {
    auto impostorForceAndContactPoint
      = getImpostorForceAndContactPoint(impostor.get());
    if (!impostorForceAndContactPoint) {
      return;
    }

    impostor->applyForce(impostorForceAndContactPoint->force,
                         impostorForceAndContactPoint->contactPoint);
  }
}

void PhysicsUpdraftEvent::_prepareCylinder()
{
  if (!_cylinder) {
    CylinderOptions options(_radius * 2.f);
    options.height = _height;
    _cylinder
      = MeshBuilder::CreateCylinder("updraftEventCylinder", options, _scene);
    _cylinder->isVisible = false;
  }
}

bool PhysicsUpdraftEvent::_intersectsWithCylinder(PhysicsImpostor* impostor)
{
  auto impostorObject = static_cast<AbstractMesh*>(impostor->object);

  _prepareCylinder();

  _cylinder->setPosition(_cylinderPosition);

  return _cylinder->intersectsMesh(impostorObject, true);
}

} // end of namespace BABYLON
