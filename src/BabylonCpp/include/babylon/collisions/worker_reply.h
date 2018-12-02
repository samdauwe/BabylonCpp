#ifndef BABYLON_COLLISIONS_WORKER_REPLY_H
#define BABYLON_COLLISIONS_WORKER_REPLY_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>
#include <babylon/collisions/collide_payload.h>
#include <babylon/collisions/collision_reply_payload.h>
#include <babylon/collisions/init_payload.h>
#include <babylon/collisions/update_payload.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT WorkerReply {
  WorkerReplyType error;
  WorkerTaskType taskType;
  InitPayload initPayload;
  CollidePayload collidePayload;
  CollisionReplyPayload collisionReplyPayload;
  UpdatePayload updatePayload;
}; // end of struct WorkerReply

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_WORKER_REPLY_H
