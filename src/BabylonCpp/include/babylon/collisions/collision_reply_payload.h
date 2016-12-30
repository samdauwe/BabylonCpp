#ifndef BABYLON_COLLISIONS_COLLISION_REPLY_PAYLOAD_H
#define BABYLON_COLLISIONS_COLLISION_REPLY_PAYLOAD_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT CollisionReplyPayload {
  Float32Array newPosition;
  unsigned int collisionId;
  unsigned int collidedMeshUniqueId;
}; // end of struct CollisionReplyPayload

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLISION_REPLY_PAYLOAD_H
