#include <babylon/physics/helper/physics_radial_explosion_event.h>

#include <babylon/collisions/picking_info.h>
#include <babylon/culling/ray.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/mesh.h>
#include <babylon/physics/helper/physics_event_data.h>
#include <babylon/physics/helper/physics_hit_data.h>
#include <babylon/physics/iphysics_enabled_object.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {

PhysicsRadialExplosionEvent::PhysicsRadialExplosionEvent(
  Scene* scene, const PhysicsRadialExplosionEventOptions& options)
    : _scene{scene}, _options{options}, _sphere{nullptr}, _dataFetched{false}
{
}

PhysicsRadialExplosionEvent::~PhysicsRadialExplosionEvent() = default;

PhysicsRadialExplosionEventData PhysicsRadialExplosionEvent::getData()
{
  _dataFetched = true;

  return {
    _sphere, // sphere
  };
}

PhysicsHitDataPtr
PhysicsRadialExplosionEvent::getImpostorHitData(PhysicsImpostor& impostor,
                                                const Vector3& origin)
{
  if (impostor.mass == 0.f) {
    return nullptr;
  }

  if (!_intersectsWithSphere(impostor, origin, _options.radius)) {
    return nullptr;
  }

  if (impostor.object->getClassName() != "Mesh"
      && impostor.object->getClassName() != "InstancedMesh") {
    return nullptr;
  }

  auto impostorObjectCenter = impostor.getObjectCenter();
  auto direction            = impostorObjectCenter.subtract(origin);

  Ray ray{origin, direction, _options.radius};
  auto hit = ray.intersectsMesh(static_cast<AbstractMesh*>(impostor.object));

  auto& contactPoint = hit.pickedPoint;
  if (!contactPoint) {
    return nullptr;
  }

  auto distanceFromOrigin = Vector3::Distance(origin, *contactPoint);

  if (distanceFromOrigin > _options.radius) {
    return nullptr;
  }

  auto multiplier
    = (_options.falloff == PhysicsRadialImpulseFalloff::Constant) ?
        _options.strength :
        _options.strength * (1.f - (distanceFromOrigin / _options.radius));

  auto force = direction.multiplyByFloats(multiplier, multiplier, multiplier);

  return std::make_shared<PhysicsHitData>(PhysicsHitData{
    force,             // force
    *contactPoint,     // contactPoint
    distanceFromOrigin // distanceFromOrigin
  });
}

void PhysicsRadialExplosionEvent::triggerAffectedImpostorsCallback(
  const std::vector<PhysicsAffectedImpostorWithData>& affectedImpostorsWithData)
{
  if (_options.affectedImpostorsCallback) {
    _options.affectedImpostorsCallback(affectedImpostorsWithData);
  }
}

void PhysicsRadialExplosionEvent::dispose(bool force)
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

void PhysicsRadialExplosionEvent::_prepareSphere()
{
  if (!_sphere) {
    _sphere = SphereBuilder::CreateSphere("radialExplosionEventSphere",
                                          _options.sphere, _scene);
    _sphere->isVisible = false;
  }
}

bool PhysicsRadialExplosionEvent::_intersectsWithSphere(
  PhysicsImpostor& impostor, const Vector3& origin, float radius)
{
  auto impostorObject = static_cast<AbstractMesh*>(impostor.object);

  _prepareSphere();

  _sphere->position = origin;
  _sphere->scaling  = Vector3(radius * 2.f, radius * 2.f, radius * 2.f);
  _sphere->_updateBoundingInfo();
  _sphere->computeWorldMatrix(true);

  return _sphere->intersectsMesh(*impostorObject, true);
}

} // end of namespace BABYLON
