#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H

#include <functional>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/loading/iscene_loader_plugin_extensions.h>

namespace BABYLON {

class AbstractMesh;
class AnimationGroup;
struct IParticleSystem;
class Scene;
class SceneLoaderProgressEvent;
class Skeleton;

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
    const std::vector<std::string>& meshesNames, Scene* scene,
    const std::string& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>&
      onProgress,
    const std::function<void(std::vector<AbstractMesh*>& meshes,
                             std::vector<IParticleSystem*>& particleSystems,
                             std::vector<Skeleton*>& skeletons,
                             std::vector<AnimationGroup*>& animationGroups)>&
      onSuccess,
    const std::function<void()>& onError)
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
  loadAsync(Scene* scene, const std::string& data, const std::string& rootUrl,
            const std::function<void(const SceneLoaderProgressEvent& event)>&
              onProgress,
            const std::function<void()>& onsuccess,
            const std::function<void()>& onerror)
    = 0;

  /**
   * The friendly name of this plugin.
   */
  std::string name;

  /**
   * The file extensions supported by this plugin.
   */
  ISceneLoaderPluginExtensions extensions;

}; // end of struct ISceneLoaderPluginAsync

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H
