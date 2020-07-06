#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H

#include <functional>
#include <memory>
#include <variant>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/loading/iscene_loader_plugin_extensions.h>

namespace BABYLON {

class AbstractMesh;
class AnimationGroup;
class AssetContainer;
class IParticleSystem;
class Scene;
class SceneLoaderProgressEvent;
class Skeleton;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;
using AnimationGroupPtr  = std::shared_ptr<AnimationGroup>;
using AssetContainerPtr  = std::shared_ptr<AssetContainer>;
using IParticleSystemPtr = std::shared_ptr<IParticleSystem>;
using SkeletonPtr        = std::shared_ptr<Skeleton>;

/**
 * @brief The loaded meshes, particle systems, skeletons, and animation groups.
 */
struct ImportedMeshes {
  std::vector<AbstractMeshPtr> meshes;
  std::vector<IParticleSystemPtr> particleSystems;
  std::vector<SkeletonPtr> skeletons;
  std::vector<AnimationGroupPtr> animationGroups;
}; // end of struct ImportedMeshes

struct BABYLON_SHARED_EXPORT ISceneLoaderPluginAsync {
  virtual ~ISceneLoaderPluginAsync() = default;
  /**
   * The friendly name of this plugin.
   */
  std::string name;

  /**
   * The file extensions supported by this plugin.
   */
  std::variant<std::string, ISceneLoaderPluginExtensions> extensions;

  /**
   * @brief Import meshes into a scene.
   * @param meshesNames An array of mesh names, a single mesh name, or empty string for all meshes
   * that filter what meshes are imported
   * @param scene The scene to import into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onProgress The callback when the load progresses
   * @param fileName Defines the name of the file to load
   * @returns The loaded meshes, particle systems, skeletons, and animation
   * groups
   */
  virtual ImportedMeshes
  importMeshAsync(const std::vector<std::string>& meshesNames, Scene* scene,
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
  virtual void
  loadAsync(Scene* scene, const std::string& data, const std::string& rootUrl,
            const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress = nullptr,
            const std::string& fileName                                                  = "")
    = 0;

  /**
   * The callback that returns true if the data can be directly loaded.
   */
  std::function<bool(const std::string& data)> canDirectLoad = nullptr;

  /**
   * The callback that allows custom handling of the root url based on the response url.
   */
  std::function<bool(const std::string& rootUrl, const std::string& responseURL)> rewriteRootURL
    = nullptr;

  /**
   * @brief Load into an asset container.
   * @param scene The scene to load into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onProgress The callback when the load progresses
   * @param fileName Defines the name of the file to load
   * @returns The loaded asset container
   */
  virtual AssetContainerPtr loadAssetContainerAsync(
    Scene* scene, const std::string& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress = nullptr,
    const std::string& fileName                                                  = "")
    = 0;

}; // end of struct ISceneLoaderPluginAsync

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_ASYNC_H
