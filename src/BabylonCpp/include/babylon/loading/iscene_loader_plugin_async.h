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

/**
 * @brief The loaded meshes, particle systems, skeletons, and animation groups.
 */
struct ImportedMeshes {
  std::vector<AbstractMesh*> meshes;
  std::vector<IParticleSystem*> particleSystems;
  std::vector<Skeleton*> skeletons;
  std::vector<AnimationGroup*> animationGroups;
}; // end of struct ImportedMeshes

struct BABYLON_SHARED_EXPORT ISceneLoaderPluginAsync {

  /**
   * @brief Import meshes into a scene.
   * @param meshesNames An array of mesh names, a single mesh name, or empty
   * string for all meshes that filter what meshes are imported
   * @param scene The scene to import into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onProgress The callback when the load progresses
   * @param fileName Defines the name of the file to load
   * @returns The loaded meshes, particle systems, skeletons, and animation
   * groups
   */
  virtual ImportedMeshes importMeshAsync(
    const std::vector<std::string>& meshesNames, Scene* scene,
    const std::string& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::string& fileName = "")
    = 0;

  /**
   * @brief Load into a scene.
   * @param scene The scene to load into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onProgress The callback when the load progresses
   * @param fileName Defines the name of the file to load
   * @returns Nothing
   */
  virtual void loadAsync(
    Scene* scene, const std::string& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::string& fileName = "")
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
