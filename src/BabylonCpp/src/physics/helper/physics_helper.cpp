#include <babylon/physics/helper/physics_helper.h>

#include <babylon/core/logging.h>
#include <babylon/engine/scene.h>
#include <babylon/physics/helper/physics_force_and_contact_point.h>
#include <babylon/physics/helper/physics_gravitational_field_event.h>
#include <babylon/physics/helper/physics_radial_explosion_event.h>
#include <babylon/physics/helper/physics_updraft_event.h>
#include <babylon/physics/helper/physics_vortex_event.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {

PhysicsHelper::PhysicsHelper(Scene* scene)
    : _scene{scene}, _physicsEngine{scene->getPhysicsEngine()}
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you can use the methods.");
  }
}

PhysicsHelper::~PhysicsHelper()
{
}

unique_ptr_t<PhysicsRadialExplosionEvent>
PhysicsHelper::applyRadialExplosionImpulse(const Vector3& origin, float radius,
                                           float strength,
                                           PhysicsRadialImpulseFalloff falloff)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call this method.");
    return nullptr;
  }

  const auto& impostors = _physicsEngine->getImpostors();
  if (impostors.empty()) {
    return nullptr;
  }

  auto event = ::std::make_unique<PhysicsRadialExplosionEvent>(_scene);

  for (const auto& impostor : impostors) {
    auto impostorForceAndContactPoint = event->getImpostorForceAndContactPoint(
      impostor.get(), origin, radius, strength, falloff);
    if (!impostorForceAndContactPoint) {
      return nullptr;
    }

    impostor->applyImpulse(impostorForceAndContactPoint->force,
                           impostorForceAndContactPoint->contactPoint);
  }

  event->dispose(false);

  return event;
}

unique_ptr_t<PhysicsRadialExplosionEvent>
PhysicsHelper::applyRadialExplosionForce(const Vector3& origin, float radius,
                                         float strength,
                                         PhysicsRadialImpulseFalloff falloff)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call the PhysicsHelper.");
    return nullptr;
  }

  const auto& impostors = _physicsEngine->getImpostors();
  if (impostors.empty()) {
    return nullptr;
  }

  auto event = ::std::make_unique<PhysicsRadialExplosionEvent>(_scene);

  for (const auto& impostor : impostors) {
    auto impostorForceAndContactPoint = event->getImpostorForceAndContactPoint(
      impostor.get(), origin, radius, strength, falloff);
    if (!impostorForceAndContactPoint) {
      return nullptr;
    }

    impostor->applyForce(impostorForceAndContactPoint->force,
                         impostorForceAndContactPoint->contactPoint);
  }

  event->dispose(false);

  return event;
}

unique_ptr_t<PhysicsGravitationalFieldEvent>
PhysicsHelper::gravitationalField(const Vector3& origin, float radius,
                                  float strength,
                                  PhysicsRadialImpulseFalloff falloff)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call the PhysicsHelper.");
    return nullptr;
  }

  const auto& impostors = _physicsEngine->getImpostors();
  if (impostors.empty()) {
    return nullptr;
  }

  auto event = ::std::make_unique<PhysicsGravitationalFieldEvent>(
    this, _scene, origin, radius, strength, falloff);

  event->dispose(false);

  return event;
}

unique_ptr_t<PhysicsUpdraftEvent>
PhysicsHelper::updraft(const Vector3& origin, float radius, float strength,
                       float height, PhysicsUpdraftMode updraftMode)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call the PhysicsHelper.");
    return nullptr;
  }

  if (_physicsEngine->getImpostors().empty()) {
    return nullptr;
  }

  auto event = ::std::make_unique<PhysicsUpdraftEvent>(
    _scene, origin, radius, strength, height, updraftMode);

  event->dispose(false);

  return event;
}

unique_ptr_t<PhysicsVortexEvent> PhysicsHelper::vortex(const Vector3& origin,
                                                       float radius,
                                                       float strength,
                                                       float height)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call the PhysicsHelper.");
    return nullptr;
  }

  if (_physicsEngine->getImpostors().empty()) {
    return nullptr;
  }

  auto event = ::std::make_unique<PhysicsVortexEvent>(_scene, origin, radius,
                                                      strength, height);

  event->dispose(false);

  return event;
}

} // end of namespace BABYLON
