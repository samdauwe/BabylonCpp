#ifndef BABYLON_ENGINE_OCCLUSION_DATA_STORAGE_H
#define BABYLON_ENGINE_OCCLUSION_DATA_STORAGE_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/abstract_mesh_constants.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _OcclusionDataStorage {

  /** Hidden */
  int occlusionInternalRetryCounter = 0;

  /** Hidden */
  bool isOcclusionQueryInProgress = false;

  /** Hidden */
  bool isOccluded = false;

  /** Hidden */
  int occlusionRetryCount = -1;

  /** Hidden */
  unsigned int occlusionType = AbstractMeshConstants::OCCLUSION_TYPE_NONE;

  /** Hidden */
  unsigned int occlusionQueryAlgorithmType
    = AbstractMeshConstants::OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE;

}; // end of struct _OcclusionDataStorage

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_OCCLUSION_DATA_STORAGE_H
