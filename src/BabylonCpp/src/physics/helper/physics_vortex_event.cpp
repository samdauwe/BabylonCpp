#include <babylon/physics/helper/physics_vortex_event.h>

#include <babylon/culling/ray.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/physics/helper/physics_force_and_contact_point.h>
#include <babylon/physics/helper/physics_vortex_event_data.h>
#include <babylon/physics/iphysics_enabled_object.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {

PhysicsVortexEvent::PhysicsVortexEvent(Scene* scene, Vector3 origin,
                                       float radius, float strength,
                                       float height)
    : _scene{scene}
    , _origin{origin}
    , _radius{radius}
    , _strength{strength}
    , _height{height}
    , _originTop{Vector3::Zero()}
    , _centripetalForceThreshold{0.7f}
    , _updraftMultiplier{0.02f}
    , _tickCallback{nullptr}
    , _cylinder{nullptr}
    , _cylinderPosition{Vector3::Zero()}
    , _dataFetched{false}

{
  _physicsEngine = _scene->getPhysicsEngine();

  _origin.addToRef(Vector3(0.f, _height / 2.f, 0.f), _cylinderPosition);
  _origin.addToRef(Vector3(0.f, _height, 0.f), _originTop);
}

PhysicsVortexEvent::~PhysicsVortexEvent()
{
}

PhysicsVortexEventData PhysicsVortexEvent::getData()
{
  _dataFetched = true;

  return {
    _cylinder // cylinder
  };
}

void PhysicsVortexEvent::enable()
{
}

void PhysicsVortexEvent::disable()
{
}

void PhysicsVortexEvent::dispose(bool force)
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
PhysicsVortexEvent::getImpostorForceAndContactPoint(PhysicsImpostor* impostor)
{
  if (impostor->mass == 0.f) {
    return nullptr;
  }

  if (!_intersectsWithCylinder(impostor)) {
    return nullptr;
  }

  if (impostor->object->getClassName() != "Mesh"
      && impostor->object->getClassName() != "InstancedMesh") {
    return nullptr;
  }

  auto impostorObjectCenter = impostor->getObjectCenter();
  // the distance to the origin as if both objects were on a plane (Y-axis)
  Vector3 originOnPlane{_origin.x, impostorObjectCenter.y, _origin.z};
  auto originToImpostorDirection = impostorObjectCenter.subtract(originOnPlane);

  Ray ray{originOnPlane, originToImpostorDirection, _radius};
  auto hit = ray.intersectsMesh(static_cast<AbstractMesh*>(impostor->object));
  auto contactPoint = hit.pickedPoint;
  if (!contactPoint) {
    return nullptr;
  }

  auto absoluteDistanceFromOrigin = hit.distance / _radius;
  auto perpendicularDirection
    = Vector3::Cross(originOnPlane, impostorObjectCenter).normalize();
  auto contactPointTmp   = *contactPoint;
  auto directionToOrigin = contactPointTmp.normalize();
  if (absoluteDistanceFromOrigin > _centripetalForceThreshold) {
    directionToOrigin = directionToOrigin.negate();
  }

  // TODO: find a more physically based solution
  float forceX = 0.f;
  float forceY = 0.f;
  float forceZ = 0.f;
  if (absoluteDistanceFromOrigin > _centripetalForceThreshold) {
    forceX = directionToOrigin.x * _strength / 8.f;
    forceY = directionToOrigin.y * _updraftMultiplier;
    forceZ = directionToOrigin.z * _strength / 8.f;
  }
  else {
    forceX = (perpendicularDirection.x + directionToOrigin.x) / 2.f;
    forceY = _originTop.y * _updraftMultiplier;
    forceZ = (perpendicularDirection.z + directionToOrigin.z) / 2.f;
  }

  Vector3 force{forceX, forceY, forceZ};
  force = force.multiplyByFloats(_strength, _strength, _strength);

  return ::std::make_unique<PhysicsForceAndContactPoint>(
    PhysicsForceAndContactPoint{
      force,               // force,
      impostorObjectCenter // contactPoint
    });
}

void PhysicsVortexEvent::_tick()
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
  };
}

void PhysicsVortexEvent::_prepareCylinder()
{
  if (!_cylinder) {
    CylinderOptions options(_radius * 2.f);
    options.height = _height;
    _cylinder
      = MeshBuilder::CreateCylinder("vortexEventCylinder", options, _scene);
    _cylinder->isVisible = false;
  }
}

bool PhysicsVortexEvent::_intersectsWithCylinder(PhysicsImpostor* impostor)
{
  auto impostorObject = static_cast<AbstractMesh*>(impostor->object);

  _prepareCylinder();

  _cylinder->position = _cylinderPosition;

  return _cylinder->intersectsMesh(impostorObject, true);
}

} // end of namespace BABYLON
