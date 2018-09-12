#ifndef BABYLON_COLLISIONS_COLLISION_COORDINATOR_WORKER_H
#define BABYLON_COLLISIONS_COLLISION_COORDINATOR_WORKER_H

#include <babylon/babylon_api.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/collisions/serialized_geometry.h>
#include <babylon/collisions/serialized_mesh.h>
#include <babylon/collisions/worker.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CollisionCoordinatorWorker
    : public ICollisionCoordinator {

public:
  CollisionCoordinatorWorker();
  virtual ~CollisionCoordinatorWorker();

  /** Statics **/
  static SerializedMesh SerializeMesh(AbstractMesh* mesh);
  static SerializedGeometry SerializeGeometry(Geometry* geometry);

  /** Methods **/
  void getNewPosition(
    Vector3& position, Vector3& displacement, Collider* collider,
    unsigned int maximumRetry, AbstractMesh* excludedMesh,
    const std::function<void(unsigned int collisionIndex, Vector3& newPosition,
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
  void _afterRender();
  void _onMessageFromWorker(const WorkerReply& returnData);

private:
  Scene* _scene;
  Vector3 _scaledPosition;
  Vector3 _scaledVelocity;
  std::vector<
    std::function<void(unsigned int collisionIndex, Vector3& newPosition,
                       AbstractMesh* collidedMesh)>>
    _collisionsCallbackArray;
  bool _init;
  int _runningUpdated;
  Worker _worker;
  std::unordered_map<unsigned int, SerializedMesh> _addUpdateMeshesList;
  std::unordered_map<std::string, SerializedGeometry> _addUpdateGeometriesList;
  Uint32Array _toRemoveMeshesArray;
  std::vector<std::string> _toRemoveGeometryArray;

}; // end of class CollisionCoordinatorWorker

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLISION_COORDINATOR_WORKER_H
