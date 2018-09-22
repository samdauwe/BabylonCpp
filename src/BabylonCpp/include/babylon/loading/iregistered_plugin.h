#ifndef BABYLON_LOADING_IREGISTERED_PLUGIN_H
#define BABYLON_LOADING_IREGISTERED_PLUGIN_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct ISceneLoaderPlugin;

struct BABYLON_SHARED_EXPORT IRegisteredPlugin {
  std::shared_ptr<ISceneLoaderPlugin> plugin;
  bool isBinary;
}; // end of struct IRegisteredPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_IREGISTERED_PLUGIN_H
