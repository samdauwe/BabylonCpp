#include <babylon/loading/scene_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/loading/ifileInfo.h>
#include <babylon/loading/iregistered_plugin.h>
#include <babylon/loading/iscene_loader_plugin.h>
#include <babylon/loading/iscene_loader_plugin_async.h>
#include <babylon/loading/iscene_loader_plugin_factory.h>
#include <babylon/loading/plugins/babylon/babylon_file_loader.h>
#include <babylon/loading/scene_loader_progress_event.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

bool SceneLoader::_ForceFullSceneLoadingForIncremental = false;
bool SceneLoader::_ShowLoadingScreen                   = true;
bool SceneLoader::_CleanBoneMatrixWeights              = false;
unsigned int SceneLoader::_loggingLevel = SceneLoader::NO_LOGGING;
Observable<ISceneLoaderPlugin> SceneLoader::OnPluginActivatedObservable;
Observable<ISceneLoaderPluginAsync>
  SceneLoader::OnPluginAsyncActivatedObservable;

bool SceneLoader::ForceFullSceneLoadingForIncremental()
{
  return SceneLoader::_ForceFullSceneLoadingForIncremental;
}

void SceneLoader::setForceFullSceneLoadingForIncremental(bool value)
{
  SceneLoader::_ForceFullSceneLoadingForIncremental = value;
}

bool SceneLoader::ShowLoadingScreen()
{
  return SceneLoader::_ShowLoadingScreen;
}

void SceneLoader::setShowLoadingScreen(bool value)
{
  SceneLoader::_ShowLoadingScreen = value;
}

unsigned int SceneLoader::LoggingLevel()
{
  return SceneLoader::_loggingLevel;
}

void SceneLoader::setLoggingLevel(unsigned int value)
{
  SceneLoader::_loggingLevel = value;
}

bool SceneLoader::CleanBoneMatrixWeights()
{
  return SceneLoader::_CleanBoneMatrixWeights;
}

void SceneLoader::setCleanBoneMatrixWeights(bool value)
{
  SceneLoader::_CleanBoneMatrixWeights = value;
}

std::unordered_map<std::string, IRegisteredPlugin>
  SceneLoader::_registeredPlugins{};

void SceneLoader::RegisterPlugins()
{
  // Register babylon.js file loader
  SceneLoader::RegisterPlugin(std::make_shared<BabylonFileLoader>());
}

IRegisteredPlugin SceneLoader::_getDefaultPlugin()
{
  // Add default plugin
  if (SceneLoader::_registeredPlugins.empty()) {
    SceneLoader::RegisterPlugin(std::make_shared<BabylonFileLoader>());
  }

  return SceneLoader::_registeredPlugins[".babylon"];
}

IRegisteredPlugin
SceneLoader::_getPluginForExtension(const std::string& extension)
{
  if (stl_util::contains(SceneLoader::_registeredPlugins, extension)) {
    return SceneLoader::_registeredPlugins[extension];
  }

  if (extension == ".babylon" && SceneLoader::_registeredPlugins.empty()) {
    SceneLoader::RegisterPlugins();
    return SceneLoader::_getDefaultPlugin();
  }

  BABYLON_LOGF_WARN(
    "SceneLoader",
    "Unable to find a plugin to load %s files. Trying to use .babylon default "
    "plugin. To load from a specific filetype (eg. gltf) see: "
    "http://doc.babylonjs.com/how_to/load_from_any_file_type",
    extension.c_str());
  return SceneLoader::_getDefaultPlugin();
}

IRegisteredPlugin SceneLoader::_getPluginForDirectLoad(const std::string& data)
{
  for (const auto& pluginItem : SceneLoader::_registeredPlugins) {
    const auto& registeredPlugin = pluginItem.second;

    if (std::holds_alternative<ISceneLoaderPluginPtr>(
          registeredPlugin.plugin)) {
      auto plugin = std::get<ISceneLoaderPluginPtr>(registeredPlugin.plugin);
      if (plugin->canDirectLoad && plugin->canDirectLoad(data)) {
        return registeredPlugin;
      }
    }
    else if (std::holds_alternative<ISceneLoaderPluginAsyncPtr>(
               registeredPlugin.plugin)) {
      auto plugin
        = std::get<ISceneLoaderPluginAsyncPtr>(registeredPlugin.plugin);
      if (plugin->canDirectLoad && plugin->canDirectLoad(data)) {
        return registeredPlugin;
      }
    }
    else if (std::holds_alternative<ISceneLoaderPluginFactoryPtr>(
               registeredPlugin.plugin)) {
      auto plugin
        = std::get<ISceneLoaderPluginFactoryPtr>(registeredPlugin.plugin);
      if (plugin->factoryCanDirectLoad && plugin->factoryCanDirectLoad(data)) {
        return registeredPlugin;
      }
    }
  }

  return SceneLoader::_getDefaultPlugin();
}

IRegisteredPlugin SceneLoader::_getPluginForFilename(std::string sceneFilename)
{
  auto queryStringPosition = sceneFilename.find("?", 0);

  if (queryStringPosition == std::string::npos) {
    sceneFilename = sceneFilename.substr(0, queryStringPosition);
  }

  auto dotPosition = sceneFilename.find_last_of(".");

  auto extension = String::toLowerCase(
    sceneFilename.substr(dotPosition, sceneFilename.size()));
  return SceneLoader::_getPluginForExtension(extension);
}

std::string SceneLoader::_getDirectLoad(const std::string& sceneFilename)
{
  if (String::startsWith(sceneFilename, "data:")) {
    return sceneFilename.substr(5);
  }

  return "";
}

std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>
SceneLoader::_loadData(
  const IFileInfo& fileInfo, Scene* scene,
  const std::function<
    void(const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>&
           plugin,
         const std::string& data, const std::string& responseURL)>& onSuccess,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError,
  const std::function<void()>& /*onDispose*/,
  const std::string& pluginExtension)
{
  auto directLoad = SceneLoader::_getDirectLoad(fileInfo.name);
  auto registeredPlugin
    = (!pluginExtension.empty()) ?
        SceneLoader::_getPluginForExtension(pluginExtension) :
        (!directLoad.empty() ?
           SceneLoader::_getPluginForDirectLoad(fileInfo.name) :
           SceneLoader::_getPluginForFilename(fileInfo.name));

  std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr> plugin;
  ISceneLoaderPluginPtr syncedPlugin       = nullptr;
  ISceneLoaderPluginAsyncPtr asyncedPlugin = nullptr;
  if (std::holds_alternative<ISceneLoaderPluginPtr>(registeredPlugin.plugin)) {
    plugin = syncedPlugin
      = std::get<ISceneLoaderPluginPtr>(registeredPlugin.plugin);
  }
  else if (std::holds_alternative<ISceneLoaderPluginAsyncPtr>(
             registeredPlugin.plugin)) {
    plugin = asyncedPlugin
      = std::get<ISceneLoaderPluginAsyncPtr>(registeredPlugin.plugin);
  }
  auto& useArrayBuffer = registeredPlugin.isBinary;

  if (syncedPlugin) {
    SceneLoader::OnPluginActivatedObservable.notifyObservers(
      syncedPlugin.get());
  }
  else {
    SceneLoader::OnPluginAsyncActivatedObservable.notifyObservers(
      asyncedPlugin.get());
  }

  const auto dataCallback
    = [&](const std::variant<std::string, ArrayBuffer>& data,
          const std::string& responseURL) {
        if (scene->isDisposed()) {
          onError("Scene has been disposed", "");
          return;
        }

        onSuccess(plugin, std::get<std::string>(data), responseURL);
      };

  const auto manifestChecked = [&]() {
    std::function<void(const ProgressEvent& event)> progressCallback = nullptr;
    if (onProgress) {
      progressCallback = [&](const ProgressEvent& event) {
        onProgress(SceneLoaderProgressEvent::FromProgressEvent(event));
      };
    }

    Tools::LoadFile(
      fileInfo.url, dataCallback, progressCallback, useArrayBuffer,
      [&](const std::string& message, const std::string& exception) {
        onError("Failed to load scene."
                  + (message.empty() ? "" : " " + message),
                exception);
      });
  };

  if (!directLoad.empty()) {
    // Direct load
    dataCallback(directLoad, "");
    return plugin;
  }

  // Loading file from disk
  if (!fileInfo.name.empty()) {
    manifestChecked();
  }
  else {
    onError("Unable to find file named " + fileInfo.name, "");
  }

  return plugin;
}

std::optional<IFileInfo>
SceneLoader::_getFileInfo(std::string rootUrl, const std::string& sceneFilename)
{
  std::string url;
  std::string name;

  if (sceneFilename.empty()) {
    url     = rootUrl;
    name    = Tools::GetFilename(rootUrl);
    rootUrl = Tools::GetFolderPath(rootUrl);
  }
  else {
    if (sceneFilename.substr(0, 1) == "/") {
      BABYLON_LOG_ERROR("SceneLoader", "Wrong sceneFilename parameter");
      return std::nullopt;
    }

    url  = rootUrl + sceneFilename;
    name = sceneFilename;
  }

  return IFileInfo{
    url,     // url
    rootUrl, // rootUrl
    name,    // name
  };
}

std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr,
             ISceneLoaderPluginFactoryPtr>
SceneLoader::GetPluginForExtension(const std::string& extension)
{
  return SceneLoader::_getPluginForExtension(extension).plugin;
}

bool SceneLoader::IsPluginForExtensionAvailable(const std::string& extension)
{
  return stl_util::contains(SceneLoader::_registeredPlugins, extension);
}

void SceneLoader::RegisterPlugin(
  const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>& plugin)
{
  ISceneLoaderPluginPtr syncedPlugin       = nullptr;
  ISceneLoaderPluginAsyncPtr asyncedPlugin = nullptr;
  if (std::holds_alternative<ISceneLoaderPluginPtr>(plugin)) {
    syncedPlugin = std::get<ISceneLoaderPluginPtr>(plugin);
  }
  else {
    asyncedPlugin = std::get<ISceneLoaderPluginAsyncPtr>(plugin);
  }

  auto extensions
    = syncedPlugin ? syncedPlugin->extensions : asyncedPlugin->extensions;
  if (std::holds_alternative<std::string>(extensions)) {
    auto extension = std::get<std::string>(extensions);
    SceneLoader::_registeredPlugins[String::toLowerCase(extension)] = {
      plugin, // plugin
      false   // isBinary
    };
  }
  else {
    auto extensionList = std::get<ISceneLoaderPluginExtensions>(extensions);
    for (auto& item : extensionList.mapping) {
      SceneLoader::_registeredPlugins[String::toLowerCase(item.first)] = {
        plugin,     // plugin
        item.second // isBinary
      };
    }
  }
}

std::optional<std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>>
SceneLoader::ImportMesh(
  const std::vector<std::string>& meshNames, std::string rootUrl,
  std::string sceneFilename, Scene* scene,
  const std::function<
    void(const std::vector<AbstractMeshPtr>& meshes,
         const std::vector<IParticleSystemPtr>& particleSystems,
         const std::vector<SkeletonPtr>& skeletons,
         const std::vector<AnimationGroupPtr>& animationGroups)>& onSuccess,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::function<void(Scene* scene, const std::string& message,
                           const std::string& exception)>& onError,
  const std::string& pluginExtension)
{
  scene = scene ? scene : Engine::LastCreatedScene();

  if (!scene) {
    BABYLON_LOG_ERROR("SceneLoader", "No scene available to import mesh to");
    return std::nullopt;
  }

  auto fileInfo = SceneLoader::_getFileInfo(rootUrl, sceneFilename);
  if (!fileInfo) {
    return std::nullopt;
  }

  const auto disposeHandler = []() {};

  const auto errorHandler
    = [&](const std::string& message, const std::string& exception) {
        const auto errorMessage
          = String::concat("Unable to import meshes from ", fileInfo->url,
                           (!message.empty() ? ": " + message : ""));

        if (onError) {
          onError(scene, errorMessage, exception);
        }
        else {
          BABYLON_LOG_ERROR("SceneLoader", errorMessage);
          // should the exception be thrown?
        }

        disposeHandler();
      };

  std::function<void(const SceneLoaderProgressEvent& event)> progressHandler
    = nullptr;
  if (onProgress) {
    progressHandler = [&](const SceneLoaderProgressEvent& event) {
      try {
        onProgress(event);
      }
      catch (const std::exception& e) {
        errorHandler("Error in onProgress callback", e.what());
      }
    };
  }

  const auto successHandler
    = [&](const std::vector<AbstractMeshPtr>& meshes,
          const std::vector<IParticleSystemPtr>& particleSystems,
          const std::vector<SkeletonPtr>& skeletons,
          const std::vector<AnimationGroupPtr>& animationGroups) {
        scene->importedMeshesFiles.emplace_back(fileInfo->url);

        if (onSuccess) {
          try {
            onSuccess(meshes, particleSystems, skeletons, animationGroups);
          }
          catch (const std::exception& e) {
            errorHandler("Error in onSuccess callback", e.what());
          }
        }
      };

  return SceneLoader::_loadData(
    *fileInfo, scene,
    [&](const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>&
          plugin,
        const std::string& data, const std::string& responseURL) -> void {
      if (std::holds_alternative<ISceneLoaderPluginPtr>(plugin)) {
        auto syncedPlugin = std::get<ISceneLoaderPluginPtr>(plugin);

        if (syncedPlugin->rewriteRootURL) {
          fileInfo->rootUrl
            = syncedPlugin->rewriteRootURL(fileInfo->rootUrl, responseURL);
        }

        std::vector<AbstractMeshPtr> meshes;
        std::vector<IParticleSystemPtr> particleSystems;
        std::vector<SkeletonPtr> skeletons;
        std::vector<AnimationGroupPtr> animationGroups;

        if (!syncedPlugin->importMesh(meshNames, scene, data, fileInfo->rootUrl,
                                      meshes, particleSystems, skeletons,
                                      errorHandler)) {
          return;
        }

        scene->loadingPluginName = syncedPlugin->name;
        successHandler(meshes, particleSystems, skeletons, animationGroups);
      }
      else {
        auto asyncedPlugin = std::get<ISceneLoaderPluginAsyncPtr>(plugin);
        auto result        = asyncedPlugin->importMeshAsync(
          meshNames, scene, data, fileInfo->rootUrl, progressHandler,
          fileInfo->name);
        scene->loadingPluginName = asyncedPlugin->name;
        successHandler(result.meshes, result.particleSystems, result.skeletons,
                       result.animationGroups);
      }
    },
    progressHandler, errorHandler, disposeHandler, pluginExtension);
}

std::unique_ptr<Scene> SceneLoader::Load(
  const std::string& rootUrl, const std::string& sceneFilename, Engine* engine,
  const std::function<void(Scene* scene)>& onsuccess,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::function<void(Scene* scene, const std::string& message,
                           const std::string& exception)>& onError,
  const std::string& pluginExtension)
{
  auto scene = Scene::New(engine);
  SceneLoader::Append(rootUrl, sceneFilename, scene.get(), onsuccess,
                      onProgress, onError, pluginExtension);
  return scene;
}

std::optional<std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>>
SceneLoader::Append(
  std::string rootUrl, std::string sceneFilename, Scene* scene,
  const std::function<void(Scene* scene)>& onSuccess,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::function<void(Scene* scene, const std::string& message,
                           const std::string& exception)>& onError,
  const std::string& pluginExtension)
{
  scene = scene ? scene : Engine::LastCreatedScene();

  if (!scene) {
    BABYLON_LOG_ERROR("SceneLoader", "No scene available to append to");
    return std::nullopt;
  }

  auto fileInfo = SceneLoader::_getFileInfo(rootUrl, sceneFilename);
  if (!fileInfo) {
    return std::nullopt;
  }

  if (SceneLoader::ShowLoadingScreen()) {
    scene->getEngine()->displayLoadingUI();
  }

  const auto disposeHandler
    = [&scene]() { scene->getEngine()->hideLoadingUI(); };

  const auto errorHandler
    = [&](const std::string& message, const std::string& exception) {
        const auto errorMessage
          = String::concat("Unable to load from ", fileInfo->url,
                           (!message.empty() ? ": " + message : ""));
        if (onError) {
          onError(scene, errorMessage, exception);
        }
        else {
          BABYLON_LOG_ERROR("SceneLoader", errorMessage);
          // should the exception be thrown?
        }

        disposeHandler();
      };

  std::function<void(const SceneLoaderProgressEvent& event)> progressHandler
    = nullptr;
  if (onProgress) {
    progressHandler = [&](const SceneLoaderProgressEvent& event) {
      try {
        onProgress(event);
      }
      catch (const std::exception& e) {
        errorHandler("Error in onProgress callback", e.what());
      }
    };
  }

  const auto successHandler = [&]() {
    if (onSuccess) {
      try {
        onSuccess(scene);
      }
      catch (const std::exception& e) {
        errorHandler("Error in onSuccess callback", e.what());
      }
    }
  };

  return SceneLoader::_loadData(
    *fileInfo, scene,
    [&](const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>&
          plugin,
        const std::string& data, const std::string & /*responseURL*/) -> void {
      if (std::holds_alternative<ISceneLoaderPluginPtr>(plugin)) {
        auto syncedPlugin = std::get<ISceneLoaderPluginPtr>(plugin);
        if (!syncedPlugin->load(scene, data, fileInfo->rootUrl, errorHandler)) {
          return;
        }

        scene->loadingPluginName = syncedPlugin->name;
        successHandler();
      }
      else {
        auto asyncedPlugin = std::get<ISceneLoaderPluginAsyncPtr>(plugin);
        asyncedPlugin->loadAsync(scene, data, fileInfo->rootUrl,
                                 progressHandler, fileInfo->name);
        scene->loadingPluginName = asyncedPlugin->name;
        successHandler();
      }

      if (SceneLoader::ShowLoadingScreen()) {
        scene->executeWhenReady([](Scene* scene, EventState& /*es*/) {
          scene->getEngine()->hideLoadingUI();
        });
      }
    },
    progressHandler, errorHandler, disposeHandler, pluginExtension);
}

} // end of namespace BABYLON
