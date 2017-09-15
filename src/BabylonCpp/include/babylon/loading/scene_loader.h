#ifndef BABYLON_LOADING_SCENE_LOADER_H
#define BABYLON_LOADING_SCENE_LOADER_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SceneLoader {

public:
  // Const statics
  static constexpr unsigned int NO_LOGGING       = 0;
  static constexpr unsigned int MINIMAL_LOGGING  = 1;
  static constexpr unsigned int SUMMARY_LOGGING  = 2;
  static constexpr unsigned int DETAILED_LOGGING = 3;

  // static flag functions
  static bool ForceFullSceneLoadingForIncremental();
  static void setForceFullSceneLoadingForIncremental(bool value);
  static bool ShowLoadingScreen();
  static void setShowLoadingScreen(bool value);
  static unsigned int LoggingLevel();
  static void setLoggingLevel(unsigned int value);
  static bool CleanBoneMatrixWeights();
  static void setCleanBoneMatrixWeights(bool value);

public:
  // Flags
  static bool _ForceFullSceneLoadingForIncremental;
  static bool _ShowLoadingScreen;
  static bool _CleanBoneMatrixWeights;
  static unsigned int _loggingLevel;

private:
  // Members
  static unordered_map_t<string_t, IRegisteredPlugin> _registeredPlugins;
  // Functions
  static IRegisteredPlugin _getDefaultPlugin();
  static IRegisteredPlugin _getPluginForExtension(const string_t& extension);
  static IRegisteredPlugin _getPluginForFilename(const string_t& sceneFilename);
  static string_t _getDirectLoad(const string_t& sceneFilename);

public:
  static ISceneLoaderPlugin* GetPluginForExtension(const string_t& extension);
  static void RegisterPlugin(shared_ptr_t<ISceneLoaderPlugin>&& plugin);
  static void ImportMesh(
    const vector_t<string_t>& meshesNames, const string_t& rootUrl,
    const string_t& sceneFilename, Scene* scene,
    const ::std::function<void(vector_t<AbstractMesh*>& meshes,
                               vector_t<ParticleSystem*>& particleSystems,
                               vector_t<Skeleton*>& skeletons)>& onsuccess
    = nullptr,
    const ::std::function<void()>& progressCallBack = nullptr,
    const ::std::function<void(Scene* scene, const string_t& message,
                               const string_t& exception)>& onerror
    = nullptr);

  /**
   * Load a scene
   * @param rootUrl a string that defines the root url for scene and
   * resources
   * @param sceneFilename a string that defines the name of the scene
   * file. can start with "data:" following by the stringified version of the
   * scene
   * @param engine is the instance of BABYLON.Engine to use to create the
   * scene
   */
  unique_ptr_t<Scene>
  Load(const string_t& rootUrl, const string_t& sceneFilename, Engine* engine,
       const ::std::function<void(Scene* scene)>& onsuccess = nullptr,
       const ::std::function<void()>& progressCallBack      = nullptr,
       const ::std::function<void(Scene* scene)>& onerror   = nullptr);

  /**
   * Append a scene
   * @param rootUrl a string that defines the root url for scene and resources
   * @param sceneFilename a string that defines the name of the scene file. can
   * start with "data:" following by the stringified version of the scene
   * @param scene is the instance of BABYLON.Scene to append to
   */
  static void
  Append(const string_t& rootUrl, const string_t& sceneFilename, Scene* scene,
         const ::std::function<void(Scene* scene)>& onsuccess = nullptr,
         const ::std::function<void()>& progressCallBack      = nullptr,
         const ::std::function<void(Scene* scene)>& onerror   = nullptr);

}; // end of struct SceneLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_H
