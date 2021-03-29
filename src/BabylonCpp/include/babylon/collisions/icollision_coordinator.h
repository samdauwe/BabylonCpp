#ifndef BABYLON_COLLISIONS_ICOLLISION_COORDINATOR_H
#define BABYLON_COLLISIONS_ICOLLISION_COORDINATOR_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Scene;
class Vector3;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(Collider)

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ICollisionCoordinator {
  virtual ~ICollisionCoordinator()     = default;
  virtual ColliderPtr createCollider() = 0;
  virtual void
  getNewPosition(Vector3& position, Vector3& displacement, const ColliderPtr& collider,
                 unsigned int maximumRetry, const AbstractMeshPtr& excludedMesh,
                 const std::function<void(size_t collisionIndex, Vector3& newPosition,
                                          const AbstractMeshPtr& collidedMesh)>& onNewPosition,
                 size_t collisionIndex)
    = 0;
  virtual void init(Scene* scene) = 0;
}; // end of struct ICollisionCoordinator

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_ICOLLISION_COORDINATOR_H
