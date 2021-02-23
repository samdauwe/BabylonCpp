#ifndef BABYLON_LOADING_SCENE_LOADER_H
#define BABYLON_LOADING_SCENE_LOADER_H

#include <optional>
#include <unordered_map>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class Engine;
struct IFileInfo;
struct IRegisteredPlugin;
class Scene;
class SceneLoaderProgressEvent;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(AnimationGroup)
FWD_CLASS_SPTR(IParticleSystem)
FWD_STRUCT_SPTR(ISceneLoaderPlugin)
FWD_STRUCT_SPTR(ISceneLoaderPluginAsync)
FWD_STRUCT_SPTR(ISceneLoaderPluginFactory)
FWD_CLASS_SPTR(Geometry)
FWD_CLASS_SPTR(Light)
FWD_CLASS_SPTR(Skeleton)
FWD_CLASS_SPTR(TransformNode)

/**
 * @brief Class used to load scene from various file formats using registered plugins.
 * @see http://doc.babylonjs.com/how_to/load_from_any_file_type
 */
class BABYLON_SHARED_EXPORT SceneLoader {

public:
  /**
   * Type used for the success callback of ImportMesh
   */
  using SceneLoaderSuccessCallback = std::function<void(
    const std::vector<AbstractMeshPtr>& meshes,
    const std::vector<IParticleSystemPtr>& particleSystems,
    const std::vector<SkeletonPtr>& skeletons,
    const std::vector<AnimationGroupPtr>& animationGroups,
    const std::vector<TransformNodePtr>& transformNodes, const std::vector<GeometryPtr>& geometries,
    const std::vector<LightPtr>& lights)>;

public:
  /**
   * No logging while loading
   */
  static constexpr unsigned int NO_LOGGING = Constants::SCENELOADER_NO_LOGGING;

  /**
   * Minimal logging while loading
   */
  static constexpr unsigned int MINIMAL_LOGGING = Constants::SCENELOADER_MINIMAL_LOGGING;

  /**
   * Summary logging while loading
   */
  static constexpr unsigned int SUMMARY_LOGGING = Constants::SCENELOADER_SUMMARY_LOGGING;

  /**
   * Detailed logging while loading
   */
  static constexpr unsigned int DETAILED_LOGGING = Constants::SCENELOADER_DETAILED_LOGGING;

  /**
   * @brief Gets a boolean indicating if entire scene must be loaded even if scene contains
   * incremental data.
   */
  static bool ForceFullSceneLoadingForIncremental();

  /**
   * @brief Sets a boolean indicating if entire scene must be loaded even if scene contains
   * incremental data.
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
   * @brief Gets a boolean indicating if matrix weights must be cleaned upon loading.
   */
  static bool CleanBoneMatrixWeights();

  /**
   * @brief Sets a boolean indicating if matrix weights must be cleaned upon loading.
   */
  static void setCleanBoneMatrixWeights(bool value);

  /**
   * @brief Gets the default plugin (used to load Babylon files).
   * @returns the .babylon plugin
   */
  static IRegisteredPlugin GetDefaultPlugin();

private:
  // Functions
  static void RegisterPlugins();
  static IRegisteredPlugin _GetPluginForExtension(const std::string& extension);
  static IRegisteredPlugin _GetPluginForDirectLoad(const std::string& data);
  static IRegisteredPlugin _GetPluginForFilename(std::string sceneFilename);
  static std::string _GetDirectLoad(const std::string& sceneFilename);
  static std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr> _LoadData(
    const IFileInfo& fileInfo, Scene* scene,
    const std::function<
      void(const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>& plugin,
           const std::string& data, const std::string& responseURL)>& onSuccess,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
    const std::function<void(const std::string& message, const std::string& exception)>& onError,
    const std::function<void()>& onDispose, const std::string& pluginExtension);
  static std::shared_ptr<IFileInfo> _GetFileInfo(std::string rootUrl,
                                                 const std::string& sceneFilename);

public:
  /**
   * @brief Gets a plugin that can load the given extension.
   * @param extension defines the extension to load
   * @returns a plugin or null if none works
   */
  static std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr,
                      ISceneLoaderPluginFactoryPtr>
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
  static void
  RegisterPlugin(const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>& plugin);

  /**
   * @brief Import meshes into a scene.
   * @param meshNames an array of mesh names, a single mesh name, or empty string for all meshes
   * that filter what meshes are imported
   * @param rootUrl a string that defines the root url for the scene and resources or the
   * concatenation of rootURL and filename (e.g. http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file or starts with "data:"
   * following by the stringified version of the scene or a File object (default: empty string)
   * @param scene the instance of BABYLON.Scene to append to
   * @param onSuccess a callback with a list of imported meshes, particleSystems, skeletons, and
   * animationGroups when import succeeds
   * @param onProgress a callback with a progress event for each file being loaded
   * @param onError a callback with the scene, a message, and possibly an exception when import
   * fails
   * @param pluginExtension the extension used to determine the plugin
   * @returns The loaded plugin
   */
  static std::optional<std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>>
  ImportMesh(const std::vector<std::string>& meshNames, std::string rootUrl,
             std::string sceneFilename, Scene* scene = nullptr,
             const SceneLoaderSuccessCallback& onSuccess                                  = nullptr,
             const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress = nullptr,
             const std::function<void(Scene* scene, const std::string& message,
                                      const std::string& exception)>& onError
             = nullptr,
             const std::string& pluginExtension = "");

  /**
   * @brief Load a scene.
   * @param rootUrl a string that defines the root url for the scene and resources or the
   * concatenation of rootURL and filename (e.g. http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file or starts with "data:"
   * following by the stringified version of the scene or a File object (default: empty string)
   * @param engine is the instance of BABYLON.Engine to use to create the scene
   * @param onSuccess a callback with the scene when import succeeds
   * @param onProgress a callback with a progress event for each file being loaded
   * @param onError a callback with the scene, a message, and possibly an exception when import
   * fails
   * @param pluginExtension the extension used to determine the plugin
   * @returns The loaded plugin
   */
  static std::unique_ptr<Scene>
  Load(const std::string& rootUrl, const std::string& sceneFilename, Engine* engine,
       const std::function<void(Scene* scene)>& onsuccess                           = nullptr,
       const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress = nullptr,
       const std::function<void(Scene* scene, const std::string& message,
                                const std::string& exception)>& onError
       = nullptr,
       const std::string& pluginExtension = "");

  /**
   * @brief Append a scene.
   * @param rootUrl a string that defines the root url for the scene and resources or the
   * concatenation of rootURL and filename (e.g. http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file or starts with "data:"
   * following by the stringified version of the scene or a File object (default: empty string)
   * @param scene is the instance of BABYLON.Scene to append to
   * @param onSuccess a callback with the scene when import succeeds
   * @param onProgress a callback with a progress event for each file being loaded
   * @param onError a callback with the scene, a message, and possibly an exception when import
   * fails
   * @param pluginExtension the extension used to determine the plugin
   * @returns The loaded plugin
   */
  static std::optional<std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>>
  Append(std::string rootUrl, std::string sceneFilename = "", Scene* scene = nullptr,
         const std::function<void(Scene* scene)>& onSuccess                           = nullptr,
         const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress = nullptr,
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
  static Observable<ISceneLoaderPluginAsync> OnPluginAsyncActivatedObservable;

private:
  static std::unordered_map<std::string, IRegisteredPlugin> _registeredPlugins;

}; // end of struct SceneLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_H
