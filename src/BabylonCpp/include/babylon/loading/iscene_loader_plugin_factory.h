#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_FACTORY_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_FACTORY_H

#include <string>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/loading/iscene_loader_plugin.h>
#include <babylon/loading/iscene_loader_plugin_async.h>

namespace BABYLON {

/**
 * @brief Interface used by SceneLoader plugin factory.
 */
struct BABYLON_SHARED_EXPORT ISceneLoaderPluginFactory {
  /**
   * Defines the name of the factory
   */
  std::string name;
  /**
   * Function called to create a new plugin
   * @return the new plugin
   */
  virtual std::variant<ISceneLoaderPlugin, ISceneLoaderPluginAsync>
  createPlugin() = 0;
  /**
   * Boolean indicating if the plugin can direct load specific data
   */
  std::function<bool(const std::string& data)> canDirectLoad = nullptr;
}; // end of struct ISceneLoaderPluginFactory

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_FACTORY_H
