#ifndef BABYLON_ENGINE_OCCLUSION_DATA_STORAGE_H
#define BABYLON_ENGINE_OCCLUSION_DATA_STORAGE_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/abstract_mesh_constants.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _OcclusionDataStorage {

  /** @hidden */
  int occlusionInternalRetryCounter = 0;

  /** @hidden */
  bool isOcclusionQueryInProgress = false;

  /** @hidden */
  bool isOccluded = false;

  /** @hidden */
  int occlusionRetryCount = -1;

  /** @hidden */
  unsigned int occlusionType = AbstractMeshConstants::OCCLUSION_TYPE_NONE;

  /** @hidden */
  unsigned int occlusionQueryAlgorithmType
    = AbstractMeshConstants::OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE;

}; // end of struct _OcclusionDataStorage

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_OCCLUSION_DATA_STORAGE_H
