#ifndef BABYLON_COLLISIONS_SERIALIZED_COLLIDER_TO_WORKER_H
#define BABYLON_COLLISIONS_SERIALIZED_COLLIDER_TO_WORKER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SerializedColliderToWorker {
  Float32Array position;
  Float32Array velocity;
  Float32Array radius;
}; // end of struct SerializedColliderToWorker

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_SERIALIZED_COLLIDER_TO_WORKER_H
