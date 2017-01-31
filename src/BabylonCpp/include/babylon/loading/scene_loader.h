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
  // Flags
  static bool ForceFullSceneLoadingForIncremental;
  static bool ShowLoadingScreen;
  static unsigned int LoggingLevel;

private:
  // Members
  static std::unordered_map<std::string, IRegisteredPlugin*> _registeredPlugins;
  // Functions
  static IRegisteredPlugin* _getDefaultPlugin();
  static IRegisteredPlugin*
  _getPluginForExtension(const std::string& extension);
  static ISceneLoaderPlugin*
  _getPluginForFilename(const std::string& sceneFilename);
  static std::string _getDirectLoad(const std::string& sceneFilename);

public:
  static ISceneLoaderPlugin*
  GetPluginForExtension(const std::string& extension);
  static void RegisterPlugin(ISceneLoaderPlugin* plugin);
  static void ImportMesh(
    const std::vector<std::string>& meshesNames, const std::string& rootUrl,
    const std::string& sceneFilename, Scene* scene,
    const std::function<void(std::vector<AbstractMesh*>& meshes,
                             std::vector<ParticleSystem*>& particleSystems,
                             std::vector<Skeleton*>& skeletons)>& onsuccess
    = nullptr,
    const std::function<void()>& progressCallBack = nullptr,
    const std::function<void(Scene* scene, const std::string& message,
                             const std::string& exception)>& onerror
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
  static void Load(const std::string& rootUrl, const std::string& sceneFilename,
                   Engine* engine,
                   const std::function<void(Scene* scene)>& onsuccess = nullptr,
                   const std::function<void()>& progressCallBack      = nullptr,
                   const std::function<void(Scene* scene)>& onerror = nullptr);

  /**
   * Append a scene
   * @param rootUrl a string that defines the root url for scene and resources
   * @param sceneFilename a string that defines the name of the scene file. can
   * start with "data:" following by the stringified version of the scene
   * @param scene is the instance of BABYLON.Scene to append to
   */
  static void
  Append(const std::string& rootUrl, const std::string& sceneFilename,
         Scene* scene,
         const std::function<void(Scene* scene)>& onsuccess = nullptr,
         const std::function<void()>& progressCallBack      = nullptr,
         const std::function<void(Scene* scene)>& onerror   = nullptr);

}; // end of struct SceneLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_H
