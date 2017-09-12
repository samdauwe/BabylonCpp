#include <babylon/collisions/collision_coordinator_worker.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/babylon_message.h>
#include <babylon/collisions/collide_payload.h>
#include <babylon/collisions/serialized_collider_to_worker.h>
#include <babylon/collisions/serialized_mesh.h>
#include <babylon/collisions/serialized_sub_mesh.h>
#include <babylon/collisions/worker_reply.h>
#include <babylon/core/logging.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/instanced_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

CollisionCoordinatorWorker::CollisionCoordinatorWorker()
    : _scaledPosition{Vector3::Zero()}
    , _scaledVelocity{Vector3::Zero()}
    , _init{false}
    , _runningUpdated{0}
    , _runningCollisionTask{false}
{
}

CollisionCoordinatorWorker::~CollisionCoordinatorWorker()
{
}

SerializedMesh CollisionCoordinatorWorker::SerializeMesh(AbstractMesh* mesh)
{
  std::vector<SerializedSubMesh> submeshes;
  if (!mesh->subMeshes.empty()) {
    unsigned int idx = 0;
    for (auto& sm : mesh->subMeshes) {
      SerializedSubMesh ssm;
      ssm.position      = idx;
      ssm.verticesStart = sm->verticesStart;
      ssm.verticesCount = sm->verticesCount;
      ssm.indexStart    = sm->indexStart;
      ssm.indexCount    = sm->indexCount;
      ssm.hasMaterial   = (sm->getMaterial() != nullptr);
      ssm.sphereCenter
        = sm->getBoundingInfo()->boundingSphere.centerWorld.asArray();
      ssm.sphereRadius = sm->getBoundingInfo()->boundingSphere.radiusWorld;
      ssm.boxMinimum
        = sm->getBoundingInfo()->boundingBox.minimumWorld.asArray();
      ssm.boxMaximum
        = sm->getBoundingInfo()->boundingBox.maximumWorld.asArray();
      submeshes.emplace_back(ssm);
      ++idx;
    }
  }

  std::string geometryId;
  if (mesh->type() == IReflect::Type::MESH) {
    Mesh* _mesh = dynamic_cast<Mesh*>(mesh);
    geometryId  = _mesh->geometry() ? _mesh->geometry()->id : "";
  }
  else if (mesh->type() == IReflect::Type::INSTANCEDMESH) {
    InstancedMesh* _mesh = dynamic_cast<InstancedMesh*>(mesh);
    geometryId = (_mesh->sourceMesh() && _mesh->sourceMesh()->geometry()) ?
                   _mesh->sourceMesh()->geometry()->id :
                   "";
  }

  SerializedMesh sm;
  sm.uniqueId   = mesh->uniqueId;
  sm.id         = mesh->id;
  sm.name       = mesh->name;
  sm.geometryId = geometryId;
  sm.sphereCenter
    = mesh->getBoundingInfo()->boundingSphere.centerWorld.asArray();
  sm.sphereRadius = mesh->getBoundingInfo()->boundingSphere.radiusWorld;
  sm.boxMinimum   = mesh->getBoundingInfo()->boundingBox.minimumWorld.asArray();
  sm.boxMaximum   = mesh->getBoundingInfo()->boundingBox.maximumWorld.asArray();
  sm.worldMatrixFromCache = mesh->worldMatrixFromCache().asArray();
  sm.subMeshes            = submeshes;
  sm.checkCollisions      = mesh->checkCollisions();

  return sm;
}

SerializedGeometry
CollisionCoordinatorWorker::SerializeGeometry(Geometry* geometry)
{
  SerializedGeometry sg;
  sg.id        = geometry->id;
  sg.positions = geometry->getVerticesData(VertexBuffer::PositionKind);
  sg.normals   = geometry->getVerticesData(VertexBuffer::NormalKind);
  sg.indices   = geometry->getIndices();
  return sg;
}

void CollisionCoordinatorWorker::getNewPosition(
  Vector3& position, Vector3& velocity, Collider* collider,
  unsigned int maximumRetry, AbstractMesh* excludedMesh,
  const ::std::function<void(unsigned int collisionIndex, Vector3& newPosition,
                             AbstractMesh* AbstractMesh)>& onNewPosition,
  unsigned int collisionIndex)
{
  if (!_init) {
    return;
  }
  if (collisionIndex < _collisionsCallbackArray.size()) {
    return;
  }

  position.divideToRef(collider->radius, _scaledPosition);
  velocity.divideToRef(collider->radius, _scaledVelocity);

  if (collisionIndex >= _collisionsCallbackArray.size()) {
    _collisionsCallbackArray.resize(collisionIndex + 1);
  }

  _collisionsCallbackArray[collisionIndex] = onNewPosition;

  SerializedColliderToWorker _collider;
  _collider.position = _scaledPosition.asArray();
  _collider.velocity = _scaledVelocity.asArray();
  _collider.radius   = collider->radius.asArray();

  CollidePayload payload;
  payload.collisionId  = collisionIndex;
  payload.collider     = _collider;
  payload.maximumRetry = maximumRetry;
  payload.excludedMeshUniqueId
    = excludedMesh ? static_cast<int>(excludedMesh->uniqueId) : -1;

  BabylonMessage message;
  message.collidePayload = payload;
  message.taskType       = WorkerTaskType::COLLIDE;

  _worker.postMessage(message);
}

void CollisionCoordinatorWorker::init(Scene* scene)
{
  _scene = scene;
  _scene->registerAfterRender([this]() { _afterRender(); });

  _worker.callbackHandler
    = [this](const WorkerReply e) { _onMessageFromWorker(e); };

  BabylonMessage message;
  message.taskType = WorkerTaskType::INIT;

  _worker.postMessage(message);
}

void CollisionCoordinatorWorker::destroy()
{
  _scene->unregisterAfterRender([this]() { _afterRender(); });
  _worker.terminate();
}

void CollisionCoordinatorWorker::onMeshAdded(AbstractMesh* mesh)
{
  mesh->registerAfterWorldMatrixUpdate(
    [this](AbstractMesh* mesh) { onMeshUpdated(mesh); });
  onMeshUpdated(mesh);
}

void CollisionCoordinatorWorker::onMeshUpdated(AbstractMesh* mesh)
{
  _addUpdateMeshesList[mesh->uniqueId]
    = CollisionCoordinatorWorker::SerializeMesh(mesh);
}

void CollisionCoordinatorWorker::onMeshRemoved(AbstractMesh* mesh)
{
  _toRemoveMeshesArray.emplace_back(mesh->uniqueId);
}

void CollisionCoordinatorWorker::onGeometryAdded(Geometry* geometry)
{
  geometry->onGeometryUpdated
    = [this](Geometry* geometry, unsigned int /*kind*/) {
        onGeometryUpdated(geometry);
      };
  onGeometryUpdated(geometry);
}

void CollisionCoordinatorWorker::onGeometryUpdated(Geometry* geometry)
{
  _addUpdateGeometriesList[geometry->id]
    = CollisionCoordinatorWorker::SerializeGeometry(geometry);
}

void CollisionCoordinatorWorker::onGeometryDeleted(Geometry* geometry)
{
  _toRemoveGeometryArray.emplace_back(geometry->id);
}

void CollisionCoordinatorWorker::_afterRender()
{
  if (!_init) {
    return;
  }

  if (_toRemoveGeometryArray.empty() && _toRemoveMeshesArray.empty()) {
    return;
  }

  // 5 concurrent updates were sent to the web worker and were not yet
  // processed. Abort next update.
  // TODO make sure update runs as fast as possible to be able to update 60 FPS.
  if (_runningUpdated > 4) {
    return;
  }

  ++_runningUpdated;

  UpdatePayload payload;
  payload.updatedMeshes     = _addUpdateMeshesList;
  payload.updatedGeometries = _addUpdateGeometriesList;
  payload.removedGeometries = _toRemoveGeometryArray;
  payload.removedMeshes     = _toRemoveMeshesArray;

  BabylonMessage message;
  message.updatePayload = payload;
  message.taskType      = WorkerTaskType::UPDATE;

  std::vector<ArrayBufferView> serializable;
  for (auto item : payload.updatedGeometries) {
    const std::string& id = item.first;
    if (stl_util::contains(payload.updatedGeometries, id)) {
      // prepare transferables
      serializable.emplace_back(
        ArrayBufferView(message.updatePayload.updatedGeometries[id].indices));
      serializable.emplace_back(
        ArrayBufferView(message.updatePayload.updatedGeometries[id].normals));
      serializable.emplace_back(
        ArrayBufferView(message.updatePayload.updatedGeometries[id].positions));
    }
  }

  _worker.postMessage(message, serializable);
  _addUpdateMeshesList.clear();
  _addUpdateGeometriesList.clear();
  _toRemoveGeometryArray.clear();
  _toRemoveMeshesArray.clear();
}

void CollisionCoordinatorWorker::_onMessageFromWorker(
  const WorkerReply& returnData)
{
  if (returnData.error != WorkerReplyType::SUCCESS) {
    // TODO what errors can be returned from the worker?
    BABYLON_LOG_WARN("CollisionCoordinatorWorker",
                     "error returned from worker!");
    return;
  }

  switch (returnData.taskType) {
    case WorkerTaskType::INIT: {
      _init = true;
      // Update the worked with ALL of the scene's current state
      for (auto& mesh : _scene->meshes) {
        onMeshAdded(mesh.get());
      }
      for (auto& geometry : _scene->getGeometries()) {
        onGeometryAdded(geometry.get());
      }
    } break;
    case WorkerTaskType::UPDATE:
      --_runningUpdated;
      break;
    case WorkerTaskType::COLLIDE: {
      _runningCollisionTask = false;
      const CollisionReplyPayload& returnPayload
        = returnData.collisionReplyPayload;
      if (returnPayload.collisionId >= _collisionsCallbackArray.size()) {
        return;
      }
      //_collisionsCallbackArray[returnPayload.collisionId](
      //  returnPayload.collisionId,
      //  Vector3::FromArray(returnPayload.newPosition),
      //  _scene->getMeshByUniqueID(returnPayload.collidedMeshUniqueId));
      // cleanup
      _collisionsCallbackArray[returnPayload.collisionId] = nullptr;
    } break;
  }
}

} // end of namespace BABYLON
