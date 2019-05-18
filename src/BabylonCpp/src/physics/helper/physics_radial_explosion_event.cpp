#include <babylon/physics/helper/physics_radial_explosion_event.h>

#include <babylon/collisions/picking_info.h>
#include <babylon/culling/ray.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/physics/helper/physics_force_and_contact_point.h>
#include <babylon/physics/helper/physics_radial_explosion_event_data.h>
#include <babylon/physics/iphysics_enabled_object.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {

PhysicsRadialExplosionEvent::PhysicsRadialExplosionEvent(Scene* scene)
    : _scene{scene}, _sphere{nullptr}, _dataFetched{false}
{
  _sphereOptions          = SphereOptions(1.f);
  _sphereOptions.segments = 32;
}

PhysicsRadialExplosionEvent::~PhysicsRadialExplosionEvent()
{
}

PhysicsRadialExplosionEventData PhysicsRadialExplosionEvent::getData()
{
  _dataFetched = true;

  return {
    _sphere, // sphere
    _rays    // rays
  };
}

std::unique_ptr<PhysicsForceAndContactPoint>
PhysicsRadialExplosionEvent::getImpostorForceAndContactPoint(
  PhysicsImpostor* impostor, const Vector3& origin, float radius,
  float strength, PhysicsRadialImpulseFalloff falloff)
{
  if (impostor->mass == 0.f) {
    return nullptr;
  }

  if (!_intersectsWithSphere(impostor, origin, radius)) {
    return nullptr;
  }

  if (impostor->object->getClassName() != "Mesh"
      && impostor->object->getClassName() != "InstancedMesh") {
    return nullptr;
  }

  auto impostorObjectCenter = impostor->getObjectCenter();
  auto direction            = impostorObjectCenter.subtract(origin);

  Ray ray{origin, direction, radius};
  _rays.emplace_back(ray);
  auto hit = ray.intersectsMesh(static_cast<AbstractMesh*>(impostor->object));

  auto& contactPoint = hit.pickedPoint;
  if (!contactPoint) {
    return nullptr;
  }

  auto distanceFromOrigin = Vector3::Distance(origin, *contactPoint);
  if (distanceFromOrigin > radius) {
    return nullptr;
  }

  auto multiplier = (falloff == PhysicsRadialImpulseFalloff::Constant) ?
                      strength :
                      strength * (1.f - (distanceFromOrigin / radius));

  auto force = direction.multiplyByFloats(multiplier, multiplier, multiplier);

  return std::make_unique<PhysicsForceAndContactPoint>(
    PhysicsForceAndContactPoint{
      force,        // force
      *contactPoint // contactPoint
    });
}

void PhysicsRadialExplosionEvent::dispose(bool force)
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

void PhysicsRadialExplosionEvent::_prepareSphere()
{
  if (!_sphere) {
    _sphere            = MeshBuilder::CreateSphere("radialExplosionEventSphere",
                                        _sphereOptions, _scene);
    _sphere->isVisible = false;
  }
}

bool PhysicsRadialExplosionEvent::_intersectsWithSphere(
  PhysicsImpostor* impostor, const Vector3& origin, float radius)
{
  auto impostorObject = static_cast<AbstractMesh*>(impostor->object);

  _prepareSphere();

  _sphere->position = origin;
  _sphere->scaling  = Vector3(radius * 2.f, radius * 2.f, radius * 2.f);
  _sphere->_updateBoundingInfo();
  _sphere->computeWorldMatrix(true);

  return _sphere->intersectsMesh(*impostorObject, true);
}

} // end of namespace BABYLON
