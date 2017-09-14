#ifndef BABYLON_COLLISIONS_WORKER_H
#define BABYLON_COLLISIONS_WORKER_H

#include <babylon/babylon_global.h>
#include <babylon/collisions/collision_detector_transferable.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Worker {

public:
  Worker();
  ~Worker();

  void postMessage(const BabylonMessage& message);
  void postMessage(const BabylonMessage& message,
                   const vector_t<ArrayBufferView>& serializable);
  void terminate();

public:
  ::std::function<void(const WorkerReply& e)> callbackHandler;

private:
  CollisionDetectorTransferable collisionDetector;

}; // end of struct Worker

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_WORKER_H
