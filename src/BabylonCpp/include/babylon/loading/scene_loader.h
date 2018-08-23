#ifndef BABYLON_LOADING_SCENE_LOADER_H
#define BABYLON_LOADING_SCENE_LOADER_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

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
  static IRegisteredPlugin _getPluginForExtension(const string_t& extension);
  static IRegisteredPlugin _getPluginForDirectLoad(const string_t& data);
  static IRegisteredPlugin _getPluginForFilename(string_t sceneFilename);
  static string_t _getDirectLoad(const string_t& sceneFilename);
  static shared_ptr_t<ISceneLoaderPlugin> _loadData(
    const string_t& rootUrl, const string_t& sceneFilename, Scene* scene,
    const ::std::function<void(const shared_ptr_t<ISceneLoaderPlugin>& plugin,
                               const string_t& data,
                               const string_t& responseURL)>& onSuccess,
    const ::std::function<void(const SceneLoaderProgressEvent& event)>&
      onProgress,
    const ::std::function<void(const string_t& message,
                               const string_t& exception)>& onError,
    const ::std::function<void()>& onDispose, const string_t& pluginExtension);

public:
  static ISceneLoaderPlugin* GetPluginForExtension(const string_t& extension);
  static bool IsPluginForExtensionAvailable(const string_t& extension);
  static void RegisterPlugin(shared_ptr_t<ISceneLoaderPlugin>&& plugin);

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
  static shared_ptr_t<ISceneLoaderPlugin>
  ImportMesh(const vector_t<string_t>& meshNames, string_t rootUrl,
             string_t sceneFilename, Scene* scene = nullptr,
             const ::std::function<void(
               const vector_t<AbstractMeshPtr>& meshes,
               const vector_t<IParticleSystemPtr>& particleSystems,
               const vector_t<SkeletonPtr>& skeletons,
               const vector_t<AnimationGroupPtr>& animationGroups)>& onSuccess
             = nullptr,
             const ::std::function<void(const SceneLoaderProgressEvent& event)>&
               onProgress
             = nullptr,
             const ::std::function<void(Scene* scene, const string_t& message,
                                        const string_t& exception)>& onError
             = nullptr,
             const string_t& pluginExtension = "");

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
  unique_ptr_t<Scene>
  Load(const string_t& rootUrl, const string_t& sceneFilename, Engine* engine,
       const ::std::function<void(Scene* scene)>& onsuccess = nullptr,
       const ::std::function<void(const SceneLoaderProgressEvent& event)>&
         onProgress
       = nullptr,
       const ::std::function<void(Scene* scene, const string_t& message,
                                  const string_t& exception)>& onError
       = nullptr,
       const string_t& pluginExtension = "");

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
  static shared_ptr_t<ISceneLoaderPlugin>
  Append(string_t rootUrl, string_t sceneFilename = "", Scene* scene = nullptr,
         const ::std::function<void(Scene* scene)>& onSuccess = nullptr,
         const ::std::function<void(const SceneLoaderProgressEvent& event)>&
           onProgress
         = nullptr,
         const ::std::function<void(Scene* scene, const string_t& message,
                                    const string_t& exception)>& onError
         = nullptr,
         const string_t& pluginExtension = "");

public:
  // Members
  static Observable<ISceneLoaderPlugin> OnPluginActivatedObservable;

private:
  static unordered_map_t<string_t, IRegisteredPlugin> _registeredPlugins;

}; // end of struct SceneLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_H
