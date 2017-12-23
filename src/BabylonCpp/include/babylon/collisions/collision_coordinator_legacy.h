#ifndef BABYLON_COLLISIONS_COLLISION_COORDINATOR_LEGACY_H
#define BABYLON_COLLISIONS_COLLISION_COORDINATOR_LEGACY_H

#include <babylon/babylon_global.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CollisionCoordinatorLegacy
  : public ICollisionCoordinator {

public:
  CollisionCoordinatorLegacy();
  virtual ~CollisionCoordinatorLegacy();

  void getNewPosition(Vector3& position, Vector3& displacement,
                      Collider* collider, unsigned int maximumRetry,
                      AbstractMesh* excludedMesh,
                      const ::std::function<void(
                        unsigned int collisionIndex, Vector3& newPosition,
                        AbstractMesh* AbstractMesh)>& onNewPosition,
                      unsigned int collisionIndex) override;
  void init(Scene* scene) override;
  void destroy() override;
  void onMeshAdded(AbstractMesh* mesh) override;
  void onMeshUpdated(TransformNode* transformNode) override;
  void onMeshRemoved(AbstractMesh* mesh) override;
  void onGeometryAdded(Geometry* geometry) override;
  void onGeometryUpdated(Geometry* geometry) override;
  void onGeometryDeleted(Geometry* geometry) override;

private:
  void _collideWithWorld(Vector3& position, Vector3& velocity,
                         Collider* collider, unsigned int maximumRetry,
                         Vector3& finalPosition,
                         AbstractMesh* excludedMesh = nullptr);

private:
  Scene* _scene;
  Vector3 _scaledPosition;
  Vector3 _scaledVelocity;
  Vector3 _finalPosition;

}; // end of class CollisionCoordinatorLegacy

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLISION_COORDINATOR_LEGACY_H
