#ifndef BABYLON_LOADING_SCENE_LOADER_H
#define BABYLON_LOADING_SCENE_LOADER_H

#include <babylon/babylon_api.h>
#include <babylon/loading/iscene_loader_plugin.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class AnimationGroup;
class Engine;
struct IRegisteredPlugin;
class SceneLoaderProgressEvent;
using AnimationGroupPtr = std::shared_ptr<AnimationGroup>;

class BABYLON_SHARED_EXPORT SceneLoader {

private:
  // Flags
  static bool _ForceFullSceneLoadingForIncremental;
  static bool _ShowLoadingScreen;
  static bool _CleanBoneMatrixWeights;
  static unsigned int _loggingLevel;

public:
  static constexpr unsigned int NO_LOGGING()
  {
    return 0;
  }

  static constexpr unsigned int MINIMAL_LOGGING()
  {
    return 1;
  }

  static constexpr unsigned int SUMMARY_LOGGING()
  {
    return 2;
  }

  static constexpr unsigned int DETAILED_LOGGING()
  {
    return 3;
  }

  // static flag functions
  static bool ForceFullSceneLoadingForIncremental();
  static void setForceFullSceneLoadingForIncremental(bool value);
  static bool ShowLoadingScreen();
  static void setShowLoadingScreen(bool value);
  static unsigned int LoggingLevel();
  static void setLoggingLevel(unsigned int value);
  static bool CleanBoneMatrixWeights();
  static void setCleanBoneMatrixWeights(bool value);

private:
  // Functions
  static IRegisteredPlugin _getDefaultPlugin();
  static IRegisteredPlugin _getPluginForExtension(const std::string& extension);
  static IRegisteredPlugin _getPluginForDirectLoad(const std::string& data);
  static IRegisteredPlugin _getPluginForFilename(std::string sceneFilename);
  static std::string _getDirectLoad(const std::string& sceneFilename);
  static std::shared_ptr<ISceneLoaderPlugin> _loadData(
    const std::string& rootUrl, const std::string& sceneFilename, Scene* scene,
    const ::std::function<
      void(const std::shared_ptr<ISceneLoaderPlugin>& plugin,
           const std::string& data, const std::string& responseURL)>& onSuccess,
    const ::std::function<void(const SceneLoaderProgressEvent& event)>&
      onProgress,
    const ::std::function<void(const std::string& message,
                               const std::string& exception)>& onError,
    const ::std::function<void()>& onDispose,
    const std::string& pluginExtension);

public:
  static ISceneLoaderPlugin*
  GetPluginForExtension(const std::string& extension);
  static bool IsPluginForExtensionAvailable(const std::string& extension);
  static void RegisterPlugin(std::shared_ptr<ISceneLoaderPlugin>&& plugin);

  /**
   * @brief Import meshes into a scene.
   * @param meshNames an array of mesh names, a single mesh name, or empty
   * string for all meshes that filter what meshes are imported
   * @param rootUrl a string that defines the root url for scene and resources
   * OR the concatenation of rootURL and filename (eg.
   * http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file. can
   * start with "data:" following by the stringified version of the scene
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
    const ::std::function<
      void(const std::vector<AbstractMeshPtr>& meshes,
           const std::vector<IParticleSystemPtr>& particleSystems,
           const std::vector<SkeletonPtr>& skeletons,
           const std::vector<AnimationGroupPtr>& animationGroups)>& onSuccess
    = nullptr,
    const ::std::function<void(const SceneLoaderProgressEvent& event)>&
      onProgress
    = nullptr,
    const ::std::function<void(Scene* scene, const std::string& message,
                               const std::string& exception)>& onError
    = nullptr,
    const std::string& pluginExtension = "");

  /**
   * @brief Load a scene
   * @param rootUrl a string that defines the root url for scene and resources
   * OR the concatenation of rootURL and filename (eg.
   * http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file. can
   * start with "data:" following by the stringified version of the scene
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
  std::unique_ptr<Scene>
  Load(const std::string& rootUrl, const std::string& sceneFilename,
       Engine* engine,
       const ::std::function<void(Scene* scene)>& onsuccess = nullptr,
       const ::std::function<void(const SceneLoaderProgressEvent& event)>&
         onProgress
       = nullptr,
       const ::std::function<void(Scene* scene, const std::string& message,
                                  const std::string& exception)>& onError
       = nullptr,
       const std::string& pluginExtension = "");

  /**
   * @brief Append a scene.
   * @param rootUrl a string that defines the root url for scene and resources
   * OR the concatenation of rootURL and filename (eg.
   * http://example.com/test.glb)
   * @param sceneFilename a string that defines the name of the scene file. can
   * start with "data:" following by the stringified version of the scene
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
  static std::shared_ptr<ISceneLoaderPlugin>
  Append(std::string rootUrl, std::string sceneFilename = "",
         Scene* scene                                         = nullptr,
         const ::std::function<void(Scene* scene)>& onSuccess = nullptr,
         const ::std::function<void(const SceneLoaderProgressEvent& event)>&
           onProgress
         = nullptr,
         const ::std::function<void(Scene* scene, const std::string& message,
                                    const std::string& exception)>& onError
         = nullptr,
         const std::string& pluginExtension = "");

public:
  // Members
  static Observable<ISceneLoaderPlugin> OnPluginActivatedObservable;

private:
  static std::unordered_map<std::string, IRegisteredPlugin> _registeredPlugins;

}; // end of struct SceneLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_H
