#ifndef BABYLON_LOADING_SCENE_LOADER_H
#define BABYLON_LOADING_SCENE_LOADER_H

#include <babylon/babylon_api.h>
#include <babylon/loading/iscene_loader_plugin.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class AnimationGroup;
class Engine;
struct IFileInfo;
struct IRegisteredPlugin;
class SceneLoaderProgressEvent;
using AnimationGroupPtr = std::shared_ptr<AnimationGroup>;

/**
 * @brief Class used to load scene from various file formats using registered
 * plugins.
 * @see http://doc.babylonjs.com/how_to/load_from_any_file_type
 */
class BABYLON_SHARED_EXPORT SceneLoader {

private:
  // Flags
  static bool _ForceFullSceneLoadingForIncremental;
  static bool _ShowLoadingScreen;
  static bool _CleanBoneMatrixWeights;
  static unsigned int _loggingLevel;

public:
  /**
   * No logging while loading
   */
  static constexpr unsigned int NO_LOGGING = 0;

  /**
   * Minimal logging while loading
   */
  static constexpr unsigned int MINIMAL_LOGGING = 1;

  /**
   * Summary logging while loading
   */
  static constexpr unsigned int SUMMARY_LOGGING = 2;

  /**
   * Detailled logging while loading
   */
  static constexpr unsigned int DETAILED_LOGGING = 3;

  /**
   * @brief Gets a boolean indicating if entire scene must be loaded even if
   * scene contains incremental data.
   */
  static bool ForceFullSceneLoadingForIncremental();

  /**
   * @brief Sets a boolean indicating if entire scene must be loaded even if
   * scene contains incremental data.
   */
  static void setForceFullSceneLoadingForIncremental(bool value);

  /**
   * @brief Gets a boolean indicating if loading screen must be displayed while
   * loading a scene.
   */
  static bool ShowLoadingScreen();

  /**
   * @brief Sets a boolean indicating if loading screen must be displayed while
   * loading a scene.
   */
  static void setShowLoadingScreen(bool value);

  /**
   * @brief Defines the current logging level (while loading the scene).
   */
  static unsigned int LoggingLevel();

  /**
   * @brief Defines the current logging level (while loading the scene).
   */
  static void setLoggingLevel(unsigned int value);

  /**
   * @brief Gets a boolean indicating if matrix weights must be cleaned upon
   * loading.
   */
  static bool CleanBoneMatrixWeights();

  /**
   * @brief Sets a boolean indicating if matrix weights must be cleaned upon
   * loading.
   */
  static void setCleanBoneMatrixWeights(bool value);

private:
  // Functions
  static void RegisterPlugins();
  static IRegisteredPlugin _getDefaultPlugin();
  static IRegisteredPlugin _getPluginForExtension(const std::string& extension);
  static IRegisteredPlugin _getPluginForDirectLoad(const std::string& data);
  static IRegisteredPlugin _getPluginForFilename(std::string sceneFilename);
  static std::string _getDirectLoad(const std::string& sceneFilename);
  static std::shared_ptr<ISceneLoaderPlugin> _loadData(
    const IFileInfo& fileInfo, Scene* scene,
    const std::function<void(const std::shared_ptr<ISceneLoaderPlugin>& plugin,
                             const std::string& data,
                             const std::string& responseURL)>& onSuccess,
    const std::function<void(const SceneLoaderProgressEvent& event)>&
      onProgress,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError,
    const std::function<void()>& onDispose, const std::string& pluginExtension);
  static std::optional<IFileInfo>
  _getFileInfo(std::string rootUrl, const std::string& sceneFilename);

public:
  /**
   * @brief Gets a plugin that can load the given extension.
   * @param extension defines the extension to load
   * @returns a plugin or null if none works
   */
  static ISceneLoaderPlugin*
  GetPluginForExtension(const std::string& extension);

  /**
   * @brief Gets a boolean indicating that the given extension can be loaded.
   * @param extension defines the extension to load
   * @returns true if the extension is supported
   */
  static bool IsPluginForExtensionAvailable(const std::string& extension);

  /**
   * @brief Adds a new plugin to the list of registered plugins.
   * @param plugin defines the plugin to add
   */
  static void RegisterPlugin(std::shared_ptr<ISceneLoaderPlugin>&& plugin);

  /**
   * @brief Import meshes into a scene
   * @param meshNames an array of mesh names, a single mesh name, or empty
   * string for all meshes that filter what meshes are imported
   * @param rootUrl a string that defines the root url for the scene and
   * resources or the concatenation of rootURL and filename (e.g.
   * http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file or
   * starts with "data:" following by the stringified version of the scene
   * (default: empty string)
   * @param scene the instance of BABYLON.Scene to append to
   * @param onSuccess a callback with a list of imported meshes,
   * particleSystems, and skeletons when import succeeds
   * @param onProgress a callback with a progress event for each file being
   * loaded
   * @param onError a callback with the scene, a message, and possibly an
   * exception when import fails
   * @param pluginExtension the extension used to determine the plugin
   * @returns The loaded plugin
   */
  static std::shared_ptr<ISceneLoaderPlugin> ImportMesh(
    const std::vector<std::string>& meshNames, std::string rootUrl,
    std::string sceneFilename, Scene* scene = nullptr,
    const std::function<
      void(const std::vector<AbstractMeshPtr>& meshes,
           const std::vector<IParticleSystemPtr>& particleSystems,
           const std::vector<SkeletonPtr>& skeletons,
           const std::vector<AnimationGroupPtr>& animationGroups)>& onSuccess
    = nullptr,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::function<void(Scene* scene, const std::string& message,
                             const std::string& exception)>& onError
    = nullptr,
    const std::string& pluginExtension = "");

  /**
   * @brief Load a scene.
   * @param rootUrl a string that defines the root url for the scene and
   * resources or the concatenation of rootURL and filename (e.g.
   * http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file or
   * starts with "data:" following by the stringified version of the scene
   * (default: empty string)
   * @param engine is the instance of BABYLON.Engine to use to create the scene
   * @param onSuccess a callback with the scene when import succeeds
   * @param onProgress a callback with a progress event for each file being
   * loaded
   * @param onError a callback with the scene, a message, and possibly an
   * exception when import fails
   * @param pluginExtension the extension used to determine the plugin
   * @returns The loaded plugin
   */
  static std::unique_ptr<Scene> Load(
    const std::string& rootUrl, const std::string& sceneFilename,
    Engine* engine,
    const std::function<void(Scene* scene)>& onsuccess = nullptr,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::function<void(Scene* scene, const std::string& message,
                             const std::string& exception)>& onError
    = nullptr,
    const std::string& pluginExtension = "");

  /**
   * @brief Append a scene.
   * @param rootUrl a string that defines the root url for the scene and
   * resources or the concatenation of rootURL and filename (e.g.
   * http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file or
   * starts with "data:" following by the stringified version of the scene
   * (default: empty string)
   * @param scene is the instance of BABYLON.Scene to append to
   * @param onSuccess a callback with the scene when import succeeds
   * @param onProgress a callback with a progress event for each file being
   * loaded
   * @param onError a callback with the scene, a message, and possibly an
   * exception when import fails
   * @param pluginExtension the extension used to determine the plugin
   * @returns The loaded plugin
   */
  static std::shared_ptr<ISceneLoaderPlugin> Append(
    std::string rootUrl, std::string sceneFilename = "", Scene* scene = nullptr,
    const std::function<void(Scene* scene)>& onSuccess = nullptr,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::function<void(Scene* scene, const std::string& message,
                             const std::string& exception)>& onError
    = nullptr,
    const std::string& pluginExtension = "");

public:
  // Members

  /**
   * Event raised when a plugin is used to load a scene
   */
  static Observable<ISceneLoaderPlugin> OnPluginActivatedObservable;

private:
  static std::unordered_map<std::string, IRegisteredPlugin> _registeredPlugins;

}; // end of struct SceneLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_H
