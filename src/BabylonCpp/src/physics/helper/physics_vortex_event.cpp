#include <babylon/physics/helper/physics_vortex_event.h>

#include <babylon/culling/ray.h>
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

PhysicsVortexEvent::PhysicsVortexEvent(Scene* scene, const Vector3& origin,
                                       const PhysicsVortexEventOptions& options)
    : _scene{scene}
    , _origin{origin}
    , _options{options}
    , _physicsEngine{nullptr}
    , _originTop{Vector3::Zero()}
    , _tickCallback{nullptr}
    , _cylinder{nullptr}
    , _cylinderPosition{Vector3::Zero()}
    , _dataFetched{false}
{
  _physicsEngine = _scene->getPhysicsEngine();

  _origin.addToRef(Vector3(0.f, _options.height / 2.f, 0.f), _cylinderPosition);
  _origin.addToRef(Vector3(0.f, _options.height, 0.f), _originTop);

  _tickCallback = [this](Scene* /*scene*/, EventState& /*es*/) -> void { _tick(); };

  _prepareCylinder();
}

PhysicsVortexEvent::~PhysicsVortexEvent() = default;

PhysicsVortexEventData PhysicsVortexEvent::getData()
{
  _dataFetched = true;

  return {
    _cylinder // cylinder
  };
}

void PhysicsVortexEvent::enable()
{
  _tick();
  _scene->registerBeforeRender(_tickCallback);
}

void PhysicsVortexEvent::disable()
{
  _scene->unregisterBeforeRender(_tickCallback);
}

void PhysicsVortexEvent::dispose(bool force)
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

std::unique_ptr<PhysicsHitData> PhysicsVortexEvent::getImpostorHitData(PhysicsImpostor& impostor)
{
  if (impostor.mass == 0.f) {
    return nullptr;
  }

  if (!_intersectsWithCylinder(impostor)) {
    return nullptr;
  }

  if (impostor.object->getClassName() != "Mesh"
      && impostor.object->getClassName() != "InstancedMesh") {
    return nullptr;
  }

  auto impostorObjectCenter = impostor.getObjectCenter();
  // the distance to the origin as if both objects were on a plane (Y-axis)
  Vector3 originOnPlane{_origin.x, impostorObjectCenter.y, _origin.z};
  auto originToImpostorDirection = impostorObjectCenter.subtract(originOnPlane);

  Ray ray{originOnPlane, originToImpostorDirection, _options.radius};
  const auto hit    = ray.intersectsMesh(static_cast<AbstractMesh*>(impostor.object));
  auto contactPoint = hit.pickedPoint;
  if (!contactPoint) {
    return nullptr;
  }

  const auto absoluteDistanceFromOrigin = hit.distance / _options.radius;

  auto directionToOrigin = contactPoint->normalize();
  if (absoluteDistanceFromOrigin > _options.centripetalForceThreshold) {
    directionToOrigin = directionToOrigin.negate();
  }

  auto forceX = 0.f, forceY = 0.f, forceZ = 0.f;
  if (absoluteDistanceFromOrigin > _options.centripetalForceThreshold) {
    forceX = directionToOrigin.x * _options.centripetalForceMultiplier;
    forceY = directionToOrigin.y * _options.updraftForceMultiplier;
    forceZ = directionToOrigin.z * _options.centripetalForceMultiplier;
  }
  else {
    const auto perpendicularDirection
      = Vector3::Cross(originOnPlane, impostorObjectCenter).normalize();

    forceX = (perpendicularDirection.x + directionToOrigin.x) * _options.centrifugalForceMultiplier;
    forceY = _originTop.y * _options.updraftForceMultiplier;
    forceZ = (perpendicularDirection.z + directionToOrigin.z) * _options.centrifugalForceMultiplier;
  }

  auto force = Vector3(forceX, forceY, forceZ);
  force      = force.multiplyByFloats(_options.strength, _options.strength, _options.strength);

  return std::make_unique<PhysicsHitData>(PhysicsHitData{
    force,                     // force,
    impostorObjectCenter,      // contactPoint
    absoluteDistanceFromOrigin // distanceFromOrigin
  });
}

void PhysicsVortexEvent::_tick()
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

void PhysicsVortexEvent::_prepareCylinder()
{
  if (!_cylinder) {
    CylinderOptions options;
    options.height       = _options.height;
    options.diameter     = _options.radius * 2.f;
    _cylinder            = CylinderBuilder::CreateCylinder("vortexEventCylinder", options, _scene);
    _cylinder->isVisible = false;
  }
}

bool PhysicsVortexEvent::_intersectsWithCylinder(PhysicsImpostor& impostor)
{
  const auto impostorObject = static_cast<AbstractMesh*>(impostor.object);

  _cylinder->position = _cylinderPosition;

  return _cylinder->intersectsMesh(*impostorObject, true);
}

} // end of namespace BABYLON
