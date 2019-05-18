#ifndef BABYLON_MESHES_INSTANCE_DATA_STORAGE_H
#define BABYLON_MESHES_INSTANCE_DATA_STORAGE_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/path3d.h>

namespace BABYLON {

struct _InstancesBatch;
struct _VisibleInstances;
class Buffer;
using _InstancesBatchPtr   = std::shared_ptr<_InstancesBatch>;
using _VisibleInstancesPtr = std::shared_ptr<_VisibleInstances>;
using BufferPtr            = std::shared_ptr<Buffer>;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _InstanceDataStorage {
  _VisibleInstancesPtr visibleInstances = nullptr;
  Int32Array renderIdForInstances;
  _InstancesBatchPtr batchCache = nullptr;
  // let's start with a maximum of 32 instances
  unsigned int instancesBufferSize = 32 * 16 * 4;
  BufferPtr instancesBuffer        = nullptr;
  Float32Array instancesData;
  size_t overridenInstanceCount;
}; // end of struct _InstanceDataStorage

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_INSTANCE_DATA_STORAGE_H
