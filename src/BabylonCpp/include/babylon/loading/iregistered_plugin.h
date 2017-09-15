#ifndef BABYLON_LOADING_IREGISTERED_PLUGIN_H
#define BABYLON_LOADING_IREGISTERED_PLUGIN_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IRegisteredPlugin {
  shared_ptr_t<ISceneLoaderPlugin> plugin;
  bool isBinary;
}; // end of struct IRegisteredPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_IREGISTERED_PLUGIN_H
