#ifndef BABYLON_COLLISIONS_COLLISION_COORDINATOR_WORKER_H
#define BABYLON_COLLISIONS_COLLISION_COORDINATOR_WORKER_H

#include <babylon/babylon_global.h>
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
  void getNewPosition(Vector3& position, Vector3& velocity, Collider* collider,
                      unsigned int maximumRetry, AbstractMesh* excludedMesh,
                      const ::std::function<void(
                        unsigned int collisionIndex, Vector3& newPosition,
                        AbstractMesh* AbstractMesh)>& onNewPosition,
                      unsigned int collisionIndex) override;
  void init(Scene* scene) override;
  void destroy() override;
  void onMeshAdded(AbstractMesh* mesh) override;
  void onMeshUpdated(AbstractMesh* mesh) override;
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
  vector_t<::std::function<void(unsigned int collisionIndex,
                                Vector3& newPosition,
                                AbstractMesh* collidedMesh)>>
    _collisionsCallbackArray;
  bool _init;
  int _runningUpdated;
  bool _runningCollisionTask;
  Worker _worker;
  unordered_map_t<unsigned int, SerializedMesh> _addUpdateMeshesList;
  unordered_map_t<string_t, SerializedGeometry> _addUpdateGeometriesList;
  Uint32Array _toRemoveMeshesArray;
  vector_t<string_t> _toRemoveGeometryArray;

}; // end of class CollisionCoordinatorWorker

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLISION_COORDINATOR_WORKER_H
