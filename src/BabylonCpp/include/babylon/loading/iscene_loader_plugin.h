#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H

#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/loading/iscene_loader_plugin_extensions.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(AssetContainer)
FWD_CLASS_SPTR(IParticleSystem)
FWD_CLASS_SPTR(Skeleton)

/**
 * @brief Interface used to define a SceneLoader plugin.
 */
struct BABYLON_SHARED_EXPORT ISceneLoaderPlugin {
  virtual ~ISceneLoaderPlugin() = default;
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
   * @param meshes The meshes array to import into
   * @param particleSystems The particle systems array to import into
   * @param skeletons The skeletons array to import into
   * @param onError The callback when import fails
   * @returns True if successful or false otherwise
   */
  virtual bool importMesh(
    const std::vector<std::string>& meshesNames, Scene* scene, const std::string& data,
    const std::string& rootUrl, std::vector<AbstractMeshPtr>& meshes,
    std::vector<IParticleSystemPtr>& particleSystems, std::vector<SkeletonPtr>& skeletons,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr) const = 0;

  /**
   * @brief Load into a scene.
   * @param scene The scene to load into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onError The callback when import fails
   * @returns true if successful or false otherwise
   */
  virtual bool
  load(Scene* scene, const std::string& data, const std::string& rootUrl,
       const std::function<void(const std::string& message, const std::string& exception)>& onError
       = nullptr) const = 0;

  /**
   * The callback that returns true if the data can be directly loaded.
   */
  std::function<bool(const std::string& data)> canDirectLoad = nullptr;

  /**
   * The callback that allows custom handling of the root url based on the response url.
   */
  std::function<std::string(const std::string& rootUrl, const std::string& responseURL)>
    rewriteRootURL = nullptr;

  /**
   * @brief Load into an asset container.
   * @param scene The scene to load into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onError The callback when import fails
   * @returns The loaded asset container
   */
  virtual AssetContainerPtr loadAssetContainer(
    Scene* scene, const std::string& data, const std::string& rootUrl,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    bool addToScene = false) const = 0;

}; // end of struct ISceneLoaderPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H
