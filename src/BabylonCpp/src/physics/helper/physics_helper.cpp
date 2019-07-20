#include <babylon/physics/helper/physics_helper.h>

#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/physics/helper/physics_affected_impostor_with_data.h>
#include <babylon/physics/helper/physics_gravitational_field_event.h>
#include <babylon/physics/helper/physics_hit_data.h>
#include <babylon/physics/helper/physics_radial_explosion_event.h>
#include <babylon/physics/helper/physics_updraft_event.h>
#include <babylon/physics/helper/physics_vortex_event.h>
#include <babylon/physics/iphysics_engine.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {

PhysicsHelper::PhysicsHelper(Scene* scene)
    : _scene{scene}, _physicsEngine{scene->getPhysicsEngine()}
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you can use the methods.")
    return;
  }
}

PhysicsHelper::~PhysicsHelper()
{
}

std::unique_ptr<PhysicsRadialExplosionEvent>
PhysicsHelper::applyRadialExplosionImpulse(
  const Vector3& origin,
  RadiusOrPhysicsRadialExplosionEventOptions& radiusOrEventOptions,
  const std::optional<float>& strength,
  const std::optional<PhysicsRadialImpulseFalloff>& falloff)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call this method.")
    return nullptr;
  }

  const auto& impostors = _physicsEngine->getImpostors();
  if (impostors.empty()) {
    return nullptr;
  }

  if (std::holds_alternative<float>(radiusOrEventOptions)) {
    PhysicsRadialExplosionEventOptions eventOptions;
    eventOptions.radius   = std::get<float>(radiusOrEventOptions);
    eventOptions.strength = strength.value_or(eventOptions.strength);
    eventOptions.falloff  = falloff.value_or(eventOptions.falloff);
    radiusOrEventOptions  = eventOptions;
  }

  auto event = std::make_unique<PhysicsRadialExplosionEvent>(
    _scene, std::get<PhysicsRadialExplosionEventOptions>(radiusOrEventOptions));
  std::vector<PhysicsAffectedImpostorWithData> affectedImpostorsWithData;

  for (const auto& impostor : impostors) {
    auto impostorHitData = event->getImpostorHitData(*impostor, origin);
    if (!impostorHitData) {
      return nullptr;
    }

    impostor->applyForce(impostorHitData->force, impostorHitData->contactPoint);

    affectedImpostorsWithData.emplace_back(PhysicsAffectedImpostorWithData{
      impostor,       // impostor
      impostorHitData // hitData
    });
  }

  event->triggerAffectedImpostorsCallback(affectedImpostorsWithData);

  event->dispose(false);

  return event;
}

std::unique_ptr<PhysicsRadialExplosionEvent>
PhysicsHelper::applyRadialExplosionForce(
  const Vector3& origin,
  RadiusOrPhysicsRadialExplosionEventOptions& radiusOrEventOptions,
  const std::optional<float>& strength,
  const std::optional<PhysicsRadialImpulseFalloff>& falloff)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call the PhysicsHelper.")
    return nullptr;
  }

  const auto& impostors = _physicsEngine->getImpostors();
  if (impostors.empty()) {
    return nullptr;
  }

  if (std::holds_alternative<float>(radiusOrEventOptions)) {
    PhysicsRadialExplosionEventOptions eventOptions;
    eventOptions.radius   = std::get<float>(radiusOrEventOptions);
    eventOptions.strength = strength.value_or(eventOptions.strength);
    eventOptions.falloff  = falloff.value_or(eventOptions.falloff);
    radiusOrEventOptions  = eventOptions;
  }

  auto event = std::make_unique<PhysicsRadialExplosionEvent>(
    _scene, std::get<PhysicsRadialExplosionEventOptions>(radiusOrEventOptions));
  std::vector<PhysicsAffectedImpostorWithData> affectedImpostorsWithData;

  for (const auto& impostor : impostors) {
    auto impostorHitData = event->getImpostorHitData(*impostor, origin);
    if (!impostorHitData) {
      return nullptr;
    }

    impostor->applyForce(impostorHitData->force, impostorHitData->contactPoint);

    affectedImpostorsWithData.emplace_back(PhysicsAffectedImpostorWithData{
      impostor,       // impostor
      impostorHitData // hitData
    });
  }

  event->triggerAffectedImpostorsCallback(affectedImpostorsWithData);

  event->dispose(false);

  return event;
}

std::unique_ptr<PhysicsGravitationalFieldEvent>
PhysicsHelper::gravitationalField(
  const Vector3& origin,
  RadiusOrPhysicsRadialExplosionEventOptions& radiusOrEventOptions,
  const std::optional<float>& strength,
  const std::optional<PhysicsRadialImpulseFalloff>& falloff)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call the PhysicsHelper.")
    return nullptr;
  }

  const auto& impostors = _physicsEngine->getImpostors();
  if (impostors.empty()) {
    return nullptr;
  }

  if (std::holds_alternative<float>(radiusOrEventOptions)) {
    PhysicsRadialExplosionEventOptions eventOptions;
    eventOptions.radius   = std::get<float>(radiusOrEventOptions);
    eventOptions.strength = strength.value_or(eventOptions.strength);
    eventOptions.falloff  = falloff.value_or(eventOptions.falloff);
    radiusOrEventOptions  = eventOptions;
  }

  auto event = std::make_unique<PhysicsGravitationalFieldEvent>(
    this, _scene, origin,
    std::get<PhysicsRadialExplosionEventOptions>(radiusOrEventOptions));

  event->dispose(false);

  return event;
}

std::unique_ptr<PhysicsUpdraftEvent>
PhysicsHelper::updraft(const Vector3& origin,
                       RadiusOrPhysicsUpdraftEventOptions& radiusOrEventOptions,
                       const std::optional<float>& strength,
                       const std::optional<float>& height,
                       const std::optional<PhysicsUpdraftMode>& updraftMode)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call the PhysicsHelper.")
    return nullptr;
  }

  if (_physicsEngine->getImpostors().empty()) {
    return nullptr;
  }

  if (std::holds_alternative<float>(radiusOrEventOptions)) {
    PhysicsUpdraftEventOptions eventOptions;
    eventOptions.radius      = std::get<float>(radiusOrEventOptions);
    eventOptions.strength    = strength.value_or(eventOptions.strength);
    eventOptions.height      = height.value_or(eventOptions.height);
    eventOptions.updraftMode = updraftMode.value_or(eventOptions.updraftMode);
    radiusOrEventOptions     = eventOptions;
  }

  auto event = std::make_unique<PhysicsUpdraftEvent>(
    _scene, origin, std::get<PhysicsUpdraftEventOptions>(radiusOrEventOptions));

  event->dispose(false);

  return event;
}

std::unique_ptr<PhysicsVortexEvent>
PhysicsHelper::vortex(const Vector3& origin,
                      RadiusOrPhysicsVortexEventOptions& radiusOrEventOptions,
                      const std::optional<float>& strength,
                      const std::optional<float>& height)
{
  if (!_physicsEngine) {
    BABYLON_LOG_WARN("PhysicsHelper",
                     "Physics engine not enabled. Please enable the physics "
                     "before you call the PhysicsHelper.")
    return nullptr;
  }

  if (_physicsEngine->getImpostors().empty()) {
    return nullptr;
  }

  if (std::holds_alternative<float>(radiusOrEventOptions)) {
    PhysicsVortexEventOptions eventOptions;
    eventOptions.radius   = std::get<float>(radiusOrEventOptions);
    eventOptions.strength = strength.value_or(eventOptions.strength);
    eventOptions.height   = height.value_or(eventOptions.height);
    radiusOrEventOptions  = eventOptions;
  }

  auto event = std::make_unique<PhysicsVortexEvent>(
    _scene, origin, std::get<PhysicsVortexEventOptions>(radiusOrEventOptions));

  event->dispose(false);

  return event;
}

} // end of namespace BABYLON
