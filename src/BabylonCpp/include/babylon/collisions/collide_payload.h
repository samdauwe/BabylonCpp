#ifndef BABYLON_COLLISIONS_COLLIDE_PAYLOAD_H
#define BABYLON_COLLISIONS_COLLIDE_PAYLOAD_H

#include <babylon/babylon_api.h>
#include <babylon/collisions/serialized_collider_to_worker.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT CollidePayload {
  unsigned int collisionId;
  SerializedColliderToWorker collider;
  unsigned int maximumRetry;
  int excludedMeshUniqueId;
}; // end of struct CollidePayload

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLIDE_PAYLOAD_H
