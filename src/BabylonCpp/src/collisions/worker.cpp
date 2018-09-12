#include <babylon/collisions/worker.h>

#include <babylon/collisions/babylon_message.h>
#include <babylon/collisions/worker_reply.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

Worker::Worker()
{
}

Worker::~Worker()
{
}

void Worker::postMessage(const BabylonMessage& message)
{
  switch (message.taskType) {
    case WorkerTaskType::INIT:
      callbackHandler(collisionDetector.onInit(message.initPayload));
      break;
    case WorkerTaskType::COLLIDE:
      callbackHandler(collisionDetector.onCollision(message.collidePayload));
      break;
    case WorkerTaskType::UPDATE:
      callbackHandler(collisionDetector.onUpdate(message.updatePayload));
      break;
  }
}

void Worker::postMessage(const BabylonMessage& /*message*/,
                         const std::vector<ArrayBufferView>& /*serializable*/)
{
}

void Worker::terminate()
{
}

} // end of namespace BABYLON
