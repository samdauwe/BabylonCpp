#include <babylon/collisions/collision_detector_transferable.h>

#include <babylon/collisions/collide_payload.h>
#include <babylon/collisions/collide_worker.h>
#include <babylon/collisions/collider.h>
#include <babylon/collisions/collision_cache.h>
#include <babylon/collisions/worker_reply.h>

namespace BABYLON {

CollisionDetectorTransferable::CollisionDetectorTransferable()
{
}

CollisionDetectorTransferable::~CollisionDetectorTransferable()
{
}

WorkerReply
CollisionDetectorTransferable::onInit(const InitPayload& /*payload*/)
{
  _collisionCache = std::make_unique<CollisionCache>();

  WorkerReply reply;
  reply.error    = WorkerReplyType::SUCCESS;
  reply.taskType = WorkerTaskType::INIT;

  return reply;
}

WorkerReply
CollisionDetectorTransferable::onUpdate(const UpdatePayload& payload)
{
  WorkerReply reply;
  reply.error    = WorkerReplyType::SUCCESS;
  reply.taskType = WorkerTaskType::UPDATE;

  for (auto& item : payload.updatedGeometries) {
    _collisionCache->addGeometry(item.second);
  }

  for (auto& item : payload.updatedMeshes) {
    _collisionCache->addMesh(item.second);
  }

  for (auto& id : payload.removedGeometries) {
    _collisionCache->removeGeometry(id);
  }

  for (auto& uniqueId : payload.removedMeshes) {
    _collisionCache->removeMesh(uniqueId);
  }

  return reply;
}

WorkerReply
CollisionDetectorTransferable::onCollision(const CollidePayload& payload)
{
  Vector3 finalPosition = Vector3::Zero();
  // Create a new collider
  Collider* collider = new Collider();
  collider->radius   = Vector3::FromArray(payload.collider.radius);
  // Create new collide worker
  CollideWorker colliderWorker(collider, *_collisionCache.get(), finalPosition);
  Vector3 position = Vector3::FromArray(payload.collider.position);
  Vector3 velocity = Vector3::FromArray(payload.collider.velocity);
  colliderWorker.collideWithWorld(position, velocity, payload.maximumRetry,
                                  payload.excludedMeshUniqueId);
  CollisionReplyPayload replyPayload;
  replyPayload.collidedMeshUniqueId = collider->collidedMeshId,
  replyPayload.collisionId          = payload.collisionId,
  replyPayload.newPosition          = finalPosition.asArray();

  WorkerReply reply;
  reply.error                 = WorkerReplyType::SUCCESS;
  reply.taskType              = WorkerTaskType::COLLIDE;
  reply.collisionReplyPayload = replyPayload;

  return reply;
}

} // end of namespace BABYLON
