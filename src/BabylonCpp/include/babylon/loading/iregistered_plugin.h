#ifndef BABYLON_LOADING_IREGISTERED_PLUGIN_H
#define BABYLON_LOADING_IREGISTERED_PLUGIN_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct ISceneLoaderPlugin;

/**
 * @brief Defines a plugin registered by the SceneLoader.
 */
struct BABYLON_SHARED_EXPORT IRegisteredPlugin {
  /**
   * Defines the plugin to use
   */
  std::shared_ptr<ISceneLoaderPlugin> plugin;
  /**
   * Defines if the plugin supports binary data
   */
  bool isBinary;
}; // end of struct IRegisteredPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_IREGISTERED_PLUGIN_H
