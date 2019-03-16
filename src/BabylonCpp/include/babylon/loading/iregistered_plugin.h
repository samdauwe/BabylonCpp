#ifndef BABYLON_LOADING_IREGISTERED_PLUGIN_H
#define BABYLON_LOADING_IREGISTERED_PLUGIN_H

#include <memory>
#include <variant>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct ISceneLoaderPlugin;
struct ISceneLoaderPluginAsync;
struct ISceneLoaderPluginFactory;
using ISceneLoaderPluginPtr        = std::shared_ptr<ISceneLoaderPlugin>;
using ISceneLoaderPluginAsyncPtr   = std::shared_ptr<ISceneLoaderPluginAsync>;
using ISceneLoaderPluginFactoryPtr = std::shared_ptr<ISceneLoaderPluginFactory>;

/**
 * @brief Defines a plugin registered by the SceneLoader.
 */
struct BABYLON_SHARED_EXPORT IRegisteredPlugin {
  IRegisteredPlugin() : isBinary{false}
  {
  }

  IRegisteredPlugin(
    std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr> iPlugin,
    bool iIsBinary)
      : isBinary{iIsBinary}
  {
    if (std::holds_alternative<ISceneLoaderPluginPtr>(plugin)) {
      plugin = std::get<ISceneLoaderPluginPtr>(iPlugin);
    }
    else {
      plugin = std::get<ISceneLoaderPluginAsyncPtr>(iPlugin);
    }
  }

  IRegisteredPlugin(const IRegisteredPlugin& other)
      : plugin{other.plugin}, isBinary{other.isBinary}
  {
  }

  IRegisteredPlugin(IRegisteredPlugin&& other)
      : plugin{std::move(other.plugin)}, isBinary{std::move(other.isBinary)}
  {
  }

  IRegisteredPlugin& operator=(const IRegisteredPlugin& other)
  {
    if (&other != this) {
      plugin   = other.plugin;
      isBinary = other.isBinary;
    }

    return *this;
  }

  IRegisteredPlugin& operator=(IRegisteredPlugin&& other)
  {
    if (&other != this) {
      plugin   = std::move(other.plugin);
      isBinary = std::move(other.isBinary);
    }

    return *this;
  }

  ~IRegisteredPlugin()
  {
  }

  /**
   * Defines the plugin to use
   */
  std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr,
               ISceneLoaderPluginFactoryPtr>
    plugin;
  /**
   * Defines if the plugin supports binary data
   */
  bool isBinary;
}; // end of struct IRegisteredPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_IREGISTERED_PLUGIN_H
