#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H

#include <babylon/babylon_global.h>
#include <babylon/core/variant.h>
#include <babylon/loading/iscene_loader_plugin_extensions.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISceneLoaderPlugin {

  /**
   * @brief Import meshes into a scene.
   * @param meshesNames An array of mesh names, a single mesh name, or empty
   * string for all meshes that filter what meshes are imported
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
    const vector_t<string_t>& meshesNames, Scene* scene, const string_t& data,
    const string_t& rootUrl, vector_t<AbstractMesh*>& meshes,
    vector_t<ParticleSystem*>& particleSystems, vector_t<Skeleton*>& skeletons,
    const ::std::function<void(const string_t& message,
                               const string_t& exception)>& onError
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
  load(Scene* scene, const string_t& data, const string_t& rootUrl,
       const ::std::function<void(const string_t& message,
                                  const string_t& exception)>& onError
       = nullptr) const = 0;

  /**
   * The callback that returns true if the data can be directly loaded.
   */
  ::std::function<bool(const string_t& data)> canDirectLoad = nullptr;

  /**
   * The callback that allows custom handling of the root url based on the
   * response url.
   */
  ::std::function<string_t(const string_t& rootUrl,
                           const string_t& responseURL)>
    rewriteRootURL = nullptr;

  /**
   * The friendly name of this plugin.
   */
  string_t name;

  /**
   * The file extensions supported by this plugin.
   */
  ISceneLoaderPluginExtensions extensions;

}; // end of struct ISceneLoaderPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H
