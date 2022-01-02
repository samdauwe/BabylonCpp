#ifndef BABYLON_LOADING_SCENE_LOADER_FLAGS_H
#define BABYLON_LOADING_SCENE_LOADER_FLAGS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class used to represent data loading progression.
 */
class BABYLON_SHARED_EXPORT SceneLoaderFlags {

private:
  // Flags
  static bool _ForceFullSceneLoadingForIncremental;
  static bool _ShowLoadingScreen;
  static bool _CleanBoneMatrixWeights;
  static unsigned int _loggingLevel;

public:
  /**
   * @brief Gets a boolean indicating if entire scene must be loaded even if
   * scene contains incremental data.
   */
  static bool ForceFullSceneLoadingForIncremental();

  /**
   * @brief Sets a boolean indicating if entire scene must be loaded even if
   * scene contains incremental data.
   */
  static void setForceFullSceneLoadingForIncremental(bool value);

  /**
   * @brief Gets a boolean indicating if loading screen must be displayed while
   * loading a scene.
   */
  static bool ShowLoadingScreen();

  /**
   * @brief Sets a boolean indicating if loading screen must be displayed while
   * loading a scene.
   */
  static void setShowLoadingScreen(bool value);

  /**
   * @brief Defines the current logging level (while loading the scene).
   */
  static unsigned int loggingLevel();

  /**
   * @brief Defines the current logging level (while loading the scene).
   */
  static void setloggingLevel(unsigned int value);

  /**
   * @brief Gets a boolean indicating if matrix weights must be cleaned upon
   * loading.
   */
  static bool CleanBoneMatrixWeights();

  /**
   * @brief Sets a boolean indicating if matrix weights must be cleaned upon
   * loading.
   */
  static void setCleanBoneMatrixWeights(bool value);

}; // end of struct SceneLoaderFlags

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_FLAGS_H
