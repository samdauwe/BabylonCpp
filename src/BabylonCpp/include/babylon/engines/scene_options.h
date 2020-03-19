#ifndef BABYLON_ENGINES_SCENE_OPIONS_H
#define BABYLON_ENGINES_SCENE_OPIONS_H

#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Interface defining initialization parameters for Scene class.
 */
struct BABYLON_SHARED_EXPORT SceneOptions {

  /**
   * Defines that scene should keep up-to-date a map of geometry to enable fast look-up by uniqueId
   * It will improve performance when the number of geometries becomes important.
   */
  std::optional<bool> useGeometryUniqueIdsMap = std::nullopt;

  /**
   * Defines that each material of the scene should keep up-to-date a map of referencing meshes for
   * fast diposing It will improve performance when the number of mesh becomes important, but might
   * consume a bit more memory
   */
  std::optional<bool> useMaterialMeshMap = std::nullopt;

  /**
   * Defines that each mesh of the scene should keep up-to-date a map of referencing cloned meshes
   * for fast diposing It will improve performance when the number of mesh becomes important, but
   * might consume a bit more memory
   */
  std::optional<bool> useClonedMeshMap = std::nullopt;

  /**
   * Defines if the creation of the scene should impact the engine (Eg. UtilityLayer's scene)
   */
  std::optional<bool> isVirtual = std::nullopt;

}; // end of struct SceneOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_SCENE_OPIONS_H
