#ifndef BABYLON_COLLISIONS_ICOLLISION_DETECTOR_H
#define BABYLON_COLLISIONS_ICOLLISION_DETECTOR_H

#include <babylon/babylon_api.h>
#include <babylon/collisions/collide_payload.h>
#include <babylon/collisions/init_payload.h>
#include <babylon/collisions/update_payload.h>

namespace BABYLON {

struct WorkerReply;

struct BABYLON_SHARED_EXPORT ICollisionDetector {
  virtual WorkerReply onInit(const InitPayload& payload)         = 0;
  virtual WorkerReply onUpdate(const UpdatePayload& payload)     = 0;
  virtual WorkerReply onCollision(const CollidePayload& payload) = 0;
}; // end of struct ICollisionDetector

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_ICOLLISION_DETECTOR_H
