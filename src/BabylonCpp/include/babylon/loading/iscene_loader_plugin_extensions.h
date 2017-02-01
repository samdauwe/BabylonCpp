#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_EXTENSIONS_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_EXTENSIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISceneLoaderPluginExtensions {
  std::unordered_map<std::string, bool> mapping;
}; // end of struct ISceneLoaderPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_EXTENSIONS_H
