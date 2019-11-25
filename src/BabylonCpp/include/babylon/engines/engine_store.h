#ifndef BABYLON_ENGINES_ENGINE_STORE_H
#define BABYLON_ENGINES_ENGINE_STORE_H

#include <memory>
#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Engine;
class Scene;

/**
 * @brief The engine store class is responsible to hold all the instances of Engine and Scene
 * created during the life time of the application.
 */
struct BABYLON_SHARED_EXPORT EngineStore {

  /** Gets the list of created engines */
  static std::vector<Engine*> Instances;

  /** Hidden */
  static Scene* _LastCreatedScene;

  /**
   * @brief Gets the latest created engine.
   */
  static Engine* LastCreatedEngine();

  /**
   * @brief Gets the latest created scene.
   */
  static Scene* LastCreatedScene();

  /**
   * Gets or sets a global variable indicating if fallback texture must be used when a texture
   * cannot be loaded
   * @ignorenaming
   */
  static bool UseFallbackTexture;

  /**
   * Texture content used if a texture cannot loaded
   * @ignorenaming
   */
  static std::string FallbackTexture;

}; // end of struct EngineStore

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_STORE_H
