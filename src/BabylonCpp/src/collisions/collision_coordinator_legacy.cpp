#include <babylon/collisions/collision_coordinator_legacy.h>

#include <babylon/collisions/collider.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

CollisionCoordinatorLegacy::CollisionCoordinatorLegacy()
    : _scene{nullptr}
    , _scaledPosition{Vector3::Zero()}
    , _scaledVelocity{Vector3::Zero()}
    , _finalPosition{Vector3::Zero()}
{
}

CollisionCoordinatorLegacy::~CollisionCoordinatorLegacy()
{
}

void CollisionCoordinatorLegacy::getNewPosition(
  Vector3& position, Vector3& velocity, Collider* collider,
  unsigned int maximumRetry, AbstractMesh* excludedMesh,
  const std::function<void(unsigned int collisionIndex, Vector3& newPosition,
                           AbstractMesh* AbstractMesh)>& onNewPosition,
  unsigned int collisionIndex)
{
  position.divideToRef(collider->radius, _scaledPosition);
  velocity.divideToRef(collider->radius, _scaledVelocity);
  collider->collidedMesh    = nullptr;
  collider->retry           = 0;
  collider->initialVelocity = _scaledVelocity;
  collider->initialPosition = _scaledPosition;
  _collideWithWorld(_scaledPosition, _scaledVelocity, collider, maximumRetry,
                    _finalPosition, excludedMesh);

  _finalPosition.multiplyInPlace(collider->radius);
  // run the callback
  onNewPosition(collisionIndex, _finalPosition, collider->collidedMesh);
}

void CollisionCoordinatorLegacy::init(Scene* scene)
{
  _scene = scene;
}

void CollisionCoordinatorLegacy::destroy()
{
  // Legacy need no destruction method.
}

void CollisionCoordinatorLegacy::onMeshAdded(AbstractMesh* /*mesh*/)
{
  // No update in legacy mode
}

void CollisionCoordinatorLegacy::onMeshUpdated(AbstractMesh* /*mesh*/)
{
  // No update in legacy mode
}

void CollisionCoordinatorLegacy::onMeshRemoved(AbstractMesh* /*mesh*/)
{
  // No update in legacy mode
}

void CollisionCoordinatorLegacy::onGeometryAdded(Geometry* /*mesh*/)
{
  // No update in legacy mode
}

void CollisionCoordinatorLegacy::onGeometryUpdated(Geometry* /*mesh*/)
{
  // No update in legacy mode
}

void CollisionCoordinatorLegacy::onGeometryDeleted(Geometry* /*mesh*/)
{
  // No update in legacy mode
}

void CollisionCoordinatorLegacy::_collideWithWorld(
  Vector3& position, Vector3& velocity, Collider* collider,
  unsigned int maximumRetry, Vector3& finalPosition, AbstractMesh* excludedMesh)
{
  auto closeDistance = Engine::CollisionsEpsilon * 10.f;

  if (collider->retry >= maximumRetry) {
    finalPosition.copyFrom(position);
    return;
  }

  collider->_initialize(position, velocity, closeDistance);

  // Check all meshes
  for (auto& mesh : _scene->meshes) {
    if (mesh->isEnabled() && mesh->checkCollisions()
        && (!mesh->subMeshes.empty()) && mesh.get() != excludedMesh) {
      mesh->_checkCollision(collider);
    }
  }

  if (!collider->collisionFound) {
    position.addToRef(velocity, finalPosition);
    return;
  }

  if (!std_util::almost_equal(velocity.x, 0.f)
      || !std_util::almost_equal(velocity.y, 0.f)
      || !std_util::almost_equal(velocity.z, 0.f)) {
    collider->_getResponse(position, velocity);
  }

  if (velocity.length() <= closeDistance) {
    finalPosition.copyFrom(position);
    return;
  }

  ++collider->retry;
  _collideWithWorld(position, velocity, collider, maximumRetry, finalPosition,
                    excludedMesh);
}

} // end of namespace BABYLON
