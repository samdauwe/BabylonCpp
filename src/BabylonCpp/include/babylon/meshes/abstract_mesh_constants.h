#ifndef BABYLON_MESHES_ABSTRACT_MESH_CONSTANTS_H
#define BABYLON_MESHES_ABSTRACT_MESH_CONSTANTS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Groups all the abstract mesh constants in one place to ease
 * maintenance.
 */
struct BABYLON_SHARED_EXPORT AbstractMeshConstants {

  /** No occlusion */
  static constexpr unsigned int OCCLUSION_TYPE_NONE = 0;
  /** Occlusion set to optimisitic */
  static constexpr unsigned int OCCLUSION_TYPE_OPTIMISTIC = 1;
  /** Occlusion set to strict */
  static constexpr unsigned int OCCLUSION_TYPE_STRICT = 2;
  /** Use an accurante occlusion algorithm */
  static constexpr unsigned int OCCLUSION_ALGORITHM_TYPE_ACCURATE = 0;
  /** Use a conservative occlusion algorithm */
  static constexpr unsigned int OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE = 1;

  /** Default culling strategy with bounding box and bounding sphere and then
   * frustum culling */
  static constexpr unsigned int CULLINGSTRATEGY_STANDARD = 0;
  /** Culling strategy with bounding sphere only and then frustum culling */
  static constexpr unsigned int CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY = 1;

}; // end of struct AbstractMeshConstants

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_ABSTRACT_MESH_CONSTANTS_H
