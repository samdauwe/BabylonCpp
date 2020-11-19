#ifndef BABYLON_MESHES_INSTANCE_DATA_STORAGE_H
#define BABYLON_MESHES_INSTANCE_DATA_STORAGE_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/path3d.h>

namespace BABYLON {

struct _InstancesBatch;
struct _VisibleInstances;
class Buffer;
FWD_STRUCT_SPTR(_InstancesBatch)
FWD_STRUCT_SPTR(_VisibleInstances)
FWD_CLASS_SPTR(Buffer)

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
  bool isFrozen                                = false;
  _InstancesBatchPtr previousBatch             = nullptr;
  bool hardwareInstancedRendering              = false;
  std::optional<unsigned int> sideOrientation  = std::nullopt;
  bool manualUpdate                            = false;
  std::optional<unsigned int> previousRenderId = std::nullopt;
}; // end of struct _InstanceDataStorage

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_INSTANCE_DATA_STORAGE_H
