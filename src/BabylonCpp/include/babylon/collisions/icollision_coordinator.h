#ifndef BABYLON_COLLISIONS_ICOLLISION_COORDINATOR_H
#define BABYLON_COLLISIONS_ICOLLISION_COORDINATOR_H

#include <functional>

#include <babylon/babylon_api.h>

namespace BABYLON {

class AbstractMesh;
class Collider;
class Geometry;
class Scene;
class TransformNode;
class Vector3;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ICollisionCoordinator {
  virtual void getNewPosition(
    Vector3& position, Vector3& displacement, Collider* collider,
    unsigned int maximumRetry, AbstractMesh* excludedMesh,
    const std::function<void(unsigned int collisionIndex, Vector3& newPosition,
                             AbstractMesh* AbstractMesh)>& onNewPosition,
    unsigned int collisionIndex)
    = 0;
  virtual void init(Scene* scene) = 0;
  virtual void destroy()          = 0;

  // Update meshes and geometries
  virtual void onMeshAdded(AbstractMesh* mesh)             = 0;
  virtual void onMeshUpdated(TransformNode* transformNode) = 0;
  virtual void onMeshRemoved(AbstractMesh* mesh)           = 0;
  virtual void onGeometryAdded(Geometry* geometry)         = 0;
  virtual void onGeometryUpdated(Geometry* geometry)       = 0;
  virtual void onGeometryDeleted(Geometry* geometry)       = 0;
}; // end of struct ICollisionCoordinator

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_ICOLLISION_COORDINATOR_H
