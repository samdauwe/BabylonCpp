#ifndef BABYLON_COLLISIONS_BABYLON_MESSAGE_H
#define BABYLON_COLLISIONS_BABYLON_MESSAGE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>
#include <babylon/collisions/collide_payload.h>
#include <babylon/collisions/init_payload.h>
#include <babylon/collisions/update_payload.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BabylonMessage {
  WorkerTaskType taskType;
  InitPayload initPayload;
  CollidePayload collidePayload;
  UpdatePayload updatePayload;
}; // end of struct BabylonMessage

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_BABYLON_MESSAGE_H
