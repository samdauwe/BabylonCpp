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
   * Defines that scene should keep up-to-date a map of geometry to enable fast
   * look-up by Id It will improve performance when the number of geometries
   * becomes important.
   */
  std::optional<bool> useGeometryIdsMap = std::nullopt;

}; // end of struct SceneOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_SCENE_OPIONS_H
