#ifndef BABYLON_MESHES_THIN_INSTANCE_DATA_STORAGE_H
#define BABYLON_MESHES_THIN_INSTANCE_DATA_STORAGE_H

#include <memory>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

FWD_CLASS_SPTR(Buffer)

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _ThinInstanceDataStorage {
  size_t instancesCount                = 0;
  BufferPtr matrixBuffer               = nullptr;
  BufferPtr previousMatrixBuffer       = nullptr;
  Float32Array previousMatrixData      = {};
  size_t matrixBufferSize              = 32 * 16; // let's start with a maximum of 32 thin instances
  Float32Array matrixData              = {};
  std::vector<Vector3> boundingVectors = {};
  std::optional<std::vector<Matrix>> worldMatrices = std::nullopt;
}; // end of struct _ThinInstanceDataStorage

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_THIN_INSTANCE_DATA_STORAGE_H
