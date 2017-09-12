#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin_extensions.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISceneLoaderPluginAsync {
  ISceneLoaderPluginExtensions extensions; // Extension name, isBinary
  virtual bool importMeshAsync(
    const std::vector<std::string>& meshesNames, Scene* scene,
    const std::string& data, const std::string& rootUrl,
    const ::std::function<void(std::vector<AbstractMesh*>& meshes,
                               std::vector<ParticleSystem*>& particleSystems,
                               std::vector<Skeleton*>& skeletons)>& onSuccess,
    const ::std::function<void()>& onError)
    = 0;
  virtual bool loadAsync(Scene* scene, const std::string& data,
                         const std::string& rootUrl,
                         const ::std::function<void()>& onsuccess,
                         const ::std::function<void()>& onerror)
    = 0;
}; // end of struct ISceneLoaderPluginAsync

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H
