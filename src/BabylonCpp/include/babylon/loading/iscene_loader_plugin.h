#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin_extensions.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISceneLoaderPlugin {
  ISceneLoaderPluginExtensions extensions; // Extension name, isBinary
  virtual bool importMesh(const vector_t<string_t>& meshesNames, Scene* scene,
                          const string_t& data, const string_t& rootUrl,
                          vector_t<AbstractMesh*>& meshes,
                          vector_t<ParticleSystem*>& particleSystems,
                          vector_t<Skeleton*>& skeletons)
    = 0;
  virtual bool load(Scene* scene, const string_t& data, const string_t& rootUrl)
    = 0;
}; // end of struct ISceneLoaderPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H
