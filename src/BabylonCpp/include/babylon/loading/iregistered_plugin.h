#ifndef BABYLON_LOADING_IREGISTERED_PLUGIN_H
#define BABYLON_LOADING_IREGISTERED_PLUGIN_H

#include <memory>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

FWD_STRUCT_SPTR(ISceneLoaderPlugin)
FWD_STRUCT_SPTR(ISceneLoaderPluginAsync)
FWD_STRUCT_SPTR(ISceneLoaderPluginFactory)

/**
 * @brief Defines a plugin registered by the SceneLoader.
 */
struct BABYLON_SHARED_EXPORT IRegisteredPlugin {
  IRegisteredPlugin();
  IRegisteredPlugin(const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>& iPlugin,
                    bool iIsBinary);
  IRegisteredPlugin(const IRegisteredPlugin& other);
  IRegisteredPlugin(IRegisteredPlugin&& other);
  IRegisteredPlugin& operator=(const IRegisteredPlugin& other);
  IRegisteredPlugin& operator=(IRegisteredPlugin&& other);
  ~IRegisteredPlugin(); // = default

  /**
   * Defines the plugin to use
   */
  std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr, ISceneLoaderPluginFactoryPtr>
    plugin;
  /**
   * Defines if the plugin supports binary data
   */
  bool isBinary;
}; // end of struct IRegisteredPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_IREGISTERED_PLUGIN_H
