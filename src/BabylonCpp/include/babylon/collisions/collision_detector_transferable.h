#ifndef BABYLON_COLLISIONS_COLLISION_DETECTOR_TRANSFERABLE_H
#define BABYLON_COLLISIONS_COLLISION_DETECTOR_TRANSFERABLE_H

#include <babylon/babylon_api.h>
#include <babylon/collisions/collide_payload.h>
#include <babylon/collisions/icollision_detector.h>
#include <babylon/collisions/init_payload.h>
#include <babylon/collisions/update_payload.h>

namespace BABYLON {

class CollisionCache;

class BABYLON_SHARED_EXPORT CollisionDetectorTransferable
    : public ICollisionDetector {

public:
  CollisionDetectorTransferable();
  virtual ~CollisionDetectorTransferable();

  WorkerReply onInit(const InitPayload& payload) override;
  WorkerReply onUpdate(const UpdatePayload& payload) override;
  WorkerReply onCollision(const CollidePayload& payload) override;

private:
  std::unique_ptr<CollisionCache> _collisionCache;

}; // end of class ICollisionDetector

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLISION_DETECTOR_TRANSFERABLE_H
