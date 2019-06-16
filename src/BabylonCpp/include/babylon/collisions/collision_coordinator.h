#ifndef BABYLON_COLLISIONS_COLLISION_COORDINATOR_H
#define BABYLON_COLLISIONS_COLLISION_COORDINATOR_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT DefaultCollisionCoordinator
    : public ICollisionCoordinator {

public:
  DefaultCollisionCoordinator();
  ~DefaultCollisionCoordinator() override;

  void getNewPosition(
    Vector3& position, Vector3& displacement, const ColliderPtr& collider,
    unsigned int maximumRetry, const AbstractMeshPtr& excludedMesh,
    const std::function<void(size_t collisionIndex, Vector3& newPosition,
                             const AbstractMeshPtr& collidedMesh)>&
      onNewPosition,
    size_t collisionIndex) override;
  ColliderPtr createCollider() override;
  void init(Scene* scene) override;

private:
  void _collideWithWorld(Vector3& position, Vector3& velocity,
                         const ColliderPtr& collider, unsigned int maximumRetry,
                         Vector3& finalPosition,
                         const AbstractMeshPtr& excludedMesh = nullptr);

private:
  Scene* _scene;
  Vector3 _scaledPosition;
  Vector3 _scaledVelocity;
  Vector3 _finalPosition;

}; // end of class DefaultCollisionCoordinator

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLISION_COORDINATOR_H
