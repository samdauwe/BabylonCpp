#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin_extensions.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISceneLoaderPluginAsync {

  /**
   * @brief Import meshes into a scene.
   * @param meshesNames An array of mesh names, a single mesh name, or empty
   * string for all meshes that filter what meshes are imported
   * @param scene The scene to import into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onProgress The callback when the load progresses
   * @returns The loaded meshes, particle systems, skeletons, and animation
   * groups
   */
  virtual bool importMeshAsync(
    const vector_t<string_t>& meshesNames, Scene* scene, const string_t& data,
    const string_t& rootUrl,
    const ::std::function<void(const SceneLoaderProgressEvent& event)>&
      onProgress,
    const ::std::function<void(vector_t<AbstractMesh*>& meshes,
                               vector_t<IParticleSystem*>& particleSystems,
                               vector_t<Skeleton*>& skeletons,
                               vector_t<AnimationGroup*>& animationGroups)>&
      onSuccess,
    const ::std::function<void()>& onError)
    = 0;

  /**
   * @brief Load into a scene.
   * @param scene The scene to load into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onProgress The callback when the load progresses
   * @returns Nothing
   */
  virtual bool
  loadAsync(Scene* scene, const string_t& data, const string_t& rootUrl,
            const ::std::function<void(const SceneLoaderProgressEvent& event)>&
              onProgress,
            const ::std::function<void()>& onsuccess,
            const ::std::function<void()>& onerror)
    = 0;

  /**
   * The friendly name of this plugin.
   */
  string_t name;

  /**
   * The file extensions supported by this plugin.
   */
  ISceneLoaderPluginExtensions extensions;

}; // end of struct ISceneLoaderPluginAsync

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H
