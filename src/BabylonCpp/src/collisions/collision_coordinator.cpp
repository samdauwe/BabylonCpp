#include <babylon/collisions/collision_coordinator.h>

#include <babylon/collisions/collider.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

DefaultCollisionCoordinator::DefaultCollisionCoordinator()
    : _scene{nullptr}
    , _scaledPosition{Vector3::Zero()}
    , _scaledVelocity{Vector3::Zero()}
    , _finalPosition{Vector3::Zero()}
{
}

DefaultCollisionCoordinator::~DefaultCollisionCoordinator()
{
}

void DefaultCollisionCoordinator::getNewPosition(
  Vector3& position, Vector3& displacement, const ColliderPtr& collider,
  unsigned int maximumRetry, const AbstractMeshPtr& excludedMesh,
  const std::function<void(size_t collisionIndex, Vector3& newPosition,
                           const AbstractMeshPtr& AbstractMesh)>& onNewPosition,
  size_t collisionIndex)
{
  position.divideToRef(collider->_radius, _scaledPosition);
  displacement.divideToRef(collider->_radius, _scaledVelocity);
  collider->collidedMesh     = nullptr;
  collider->_retry           = 0;
  collider->_initialVelocity = _scaledVelocity;
  collider->_initialPosition = _scaledPosition;
  _collideWithWorld(_scaledPosition, _scaledVelocity, collider, maximumRetry,
                    _finalPosition, excludedMesh);

  _finalPosition.multiplyInPlace(collider->_radius);
  // run the callback
  onNewPosition(collisionIndex, _finalPosition, collider->collidedMesh);
}

ColliderPtr DefaultCollisionCoordinator::createCollider()
{
  return std::make_shared<Collider>();
}

void DefaultCollisionCoordinator::init(Scene* scene)
{
  _scene = scene;
}

void DefaultCollisionCoordinator::_collideWithWorld(
  Vector3& position, Vector3& velocity, const ColliderPtr& collider,
  unsigned int maximumRetry, Vector3& finalPosition,
  const AbstractMeshPtr& excludedMesh)
{
  auto closeDistance = Engine::CollisionsEpsilon * 10.f;

  if (collider->_retry >= maximumRetry) {
    finalPosition.copyFrom(position);
    return;
  }

  // Check if this is a mesh else camera or -1
  auto collisionMask = (excludedMesh ? excludedMesh->collisionMask() :
                                       collider->collisionMask());

  collider->_initialize(position, velocity, closeDistance);

  // Check all meshes
  for (const auto& mesh : _scene->meshes) {
    if (mesh->isEnabled() && mesh->checkCollisions && !mesh->subMeshes.empty()
        && mesh != excludedMesh
        && ((collisionMask & mesh->collisionGroup) != 0)) {
      mesh->_checkCollision(*collider);
    }
  }

  if (!collider->collisionFound) {
    position.addToRef(velocity, finalPosition);
    return;
  }

  if (velocity.x != 0.f || velocity.y != 0.f || velocity.z != 0.f) {
    collider->_getResponse(position, velocity);
  }

  if (velocity.length() <= closeDistance) {
    finalPosition.copyFrom(position);
    return;
  }

  ++collider->_retry;
  _collideWithWorld(position, velocity, collider, maximumRetry, finalPosition,
                    excludedMesh);
}

} // end of namespace BABYLON
