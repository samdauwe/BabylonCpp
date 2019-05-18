#ifndef BABYLON_MESHES_CREATION_DATA_STORAGE_H
#define BABYLON_MESHES_CREATION_DATA_STORAGE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/path3d.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _CreationDataStorage {
  std::optional<bool> closePath  = std::nullopt;
  std::optional<bool> closeArray = std::nullopt;
  IndicesArray idx;
  float dashSize;
  float gapSize;
  Path3D path3D;
  std::vector<std::vector<Vector3>> pathArray;
  float arc;
  float radius;
  unsigned int cap;
  unsigned int tessellation;
}; // end of struct _CreationDataStorage

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_CREATION_DATA_STORAGE_H
