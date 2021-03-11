#include <babylon/loading/scene_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/loading/ifileInfo.h>
#include <babylon/loading/iregistered_plugin.h>
#include <babylon/loading/iscene_loader_plugin.h>
#include <babylon/loading/iscene_loader_plugin_async.h>
#include <babylon/loading/iscene_loader_plugin_factory.h>
#include <babylon/loading/plugins/babylon/babylon_file_loader.h>
#include <babylon/loading/scene_loader_flags.h>
#include <babylon/loading/scene_loader_progress_event.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

Observable<ISceneLoaderPlugin> SceneLoader::OnPluginActivatedObservable;
Observable<ISceneLoaderPluginAsync> SceneLoader::OnPluginAsyncActivatedObservable;

bool SceneLoader::ForceFullSceneLoadingForIncremental()
{
  return SceneLoaderFlags::ForceFullSceneLoadingForIncremental();
}

void SceneLoader::setForceFullSceneLoadingForIncremental(bool value)
{
  SceneLoaderFlags::setForceFullSceneLoadingForIncremental(value);
}

bool SceneLoader::ShowLoadingScreen()
{
  return SceneLoaderFlags::ShowLoadingScreen();
}

void SceneLoader::setShowLoadingScreen(bool value)
{
  SceneLoaderFlags::setShowLoadingScreen(value);
}

unsigned int SceneLoader::LoggingLevel()
{
  return SceneLoaderFlags::loggingLevel();
}

void SceneLoader::setLoggingLevel(unsigned int value)
{
  SceneLoaderFlags::setloggingLevel(value);
}

bool SceneLoader::CleanBoneMatrixWeights()
{
  return SceneLoaderFlags::CleanBoneMatrixWeights();
}

void SceneLoader::setCleanBoneMatrixWeights(bool value)
{
  SceneLoaderFlags::setCleanBoneMatrixWeights(value);
}

std::unordered_map<std::string, IRegisteredPlugin> SceneLoader::_registeredPlugins{};

void SceneLoader::RegisterPlugins()
{
  // Register babylon.js file loader
  SceneLoader::RegisterPlugin(std::make_shared<BabylonFileLoader>());
}

IRegisteredPlugin SceneLoader::GetDefaultPlugin()
{
  // Add default plugin
  if (SceneLoader::_registeredPlugins.empty()) {
    SceneLoader::RegisterPlugin(std::make_shared<BabylonFileLoader>());
  }

  return SceneLoader::_registeredPlugins[".babylon"];
}

IRegisteredPlugin SceneLoader::_GetPluginForExtension(const std::string& extension)
{
  if (stl_util::contains(SceneLoader::_registeredPlugins, extension)) {
    return SceneLoader::_registeredPlugins[extension];
  }

  if (extension == ".babylon" && SceneLoader::_registeredPlugins.empty()) {
    SceneLoader::RegisterPlugins();
    return SceneLoader::GetDefaultPlugin();
  }

  BABYLON_LOGF_WARN("SceneLoader",
                    "Unable to find a plugin to load %s files. Trying to use .babylon default "
                    "plugin. To load from a specific filetype (eg. gltf) see: "
                    "http://doc.babylonjs.com/how_to/load_from_any_file_type",
                    extension.c_str())
  return SceneLoader::GetDefaultPlugin();
}

IRegisteredPlugin SceneLoader::_GetPluginForDirectLoad(const std::string& data)
{
  for (const auto& pluginItem : SceneLoader::_registeredPlugins) {
    const auto& registeredPlugin = pluginItem.second;

    if (std::holds_alternative<ISceneLoaderPluginPtr>(registeredPlugin.plugin)) {
      auto plugin = std::get<ISceneLoaderPluginPtr>(registeredPlugin.plugin);
      if (plugin->canDirectLoad && plugin->canDirectLoad(data)) {
        return registeredPlugin;
      }
    }
    else if (std::holds_alternative<ISceneLoaderPluginAsyncPtr>(registeredPlugin.plugin)) {
      auto plugin = std::get<ISceneLoaderPluginAsyncPtr>(registeredPlugin.plugin);
      if (plugin->canDirectLoad && plugin->canDirectLoad(data)) {
        return registeredPlugin;
      }
    }
    else if (std::holds_alternative<ISceneLoaderPluginFactoryPtr>(registeredPlugin.plugin)) {
      auto plugin = std::get<ISceneLoaderPluginFactoryPtr>(registeredPlugin.plugin);
      if (plugin->factoryCanDirectLoad && plugin->factoryCanDirectLoad(data)) {
        return registeredPlugin;
      }
    }
  }

  return SceneLoader::GetDefaultPlugin();
}

IRegisteredPlugin SceneLoader::_GetPluginForFilename(std::string sceneFilename)
{
  auto queryStringPosition = sceneFilename.find("?", 0);

  if (queryStringPosition == std::string::npos) {
    sceneFilename = sceneFilename.substr(0, queryStringPosition);
  }

  auto dotPosition = sceneFilename.find_last_of(".");

  auto extension
    = StringTools::toLowerCase(sceneFilename.substr(dotPosition, sceneFilename.size()));
  return SceneLoader::_GetPluginForExtension(extension);
}

std::string SceneLoader::_GetDirectLoad(const std::string& sceneFilename)
{
  if (StringTools::startsWith(sceneFilename, "data:")) {
    return sceneFilename.substr(5);
  }

  return "";
}

std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr> SceneLoader::_LoadData(
  const IFileInfo& fileInfo, Scene* scene,
  const std::function<
    void(const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>& plugin,
         const std::string& data, const std::string& responseURL)>& onSuccess,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  const std::function<void()>& /*onDispose*/, const std::string& pluginExtension)
{
  auto directLoad = SceneLoader::_GetDirectLoad(fileInfo.url);
  auto registeredPlugin
    = (!pluginExtension.empty()) ?
        SceneLoader::_GetPluginForExtension(pluginExtension) :
        (!directLoad.empty() ? SceneLoader::_GetPluginForDirectLoad(fileInfo.url) :
                               SceneLoader::_GetPluginForFilename(fileInfo.url));

  std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr> plugin;
  ISceneLoaderPluginPtr syncedPlugin       = nullptr;
  ISceneLoaderPluginAsyncPtr asyncedPlugin = nullptr;
  if (std::holds_alternative<ISceneLoaderPluginPtr>(registeredPlugin.plugin)) {
    plugin = syncedPlugin = std::get<ISceneLoaderPluginPtr>(registeredPlugin.plugin);
  }
  else if (std::holds_alternative<ISceneLoaderPluginAsyncPtr>(registeredPlugin.plugin)) {
    plugin = asyncedPlugin = std::get<ISceneLoaderPluginAsyncPtr>(registeredPlugin.plugin);
  }
  else {
    throw std::runtime_error(
      "The loader plugin corresponding to the file type you are trying to load "
      "has not been found. If using es6, please import the plugin you wish to "
      "use before.");
  }

  if (syncedPlugin) {
    SceneLoader::OnPluginActivatedObservable.notifyObservers(syncedPlugin.get());
  }
  else {
    SceneLoader::OnPluginAsyncActivatedObservable.notifyObservers(asyncedPlugin.get());
  }

  auto& useArrayBuffer = registeredPlugin.isBinary;

  const auto dataCallback
    = [scene, onError, onSuccess, plugin](const std::variant<std::string, ArrayBufferView>& data,
                                          const std::string& responseURL) -> void {
    if (scene->isDisposed()) {
      onError("Scene has been disposed", "");
      return;
    }

    onSuccess(plugin, std::get<std::string>(data), responseURL);
  };

  const auto manifestChecked
    = [fileInfo, dataCallback, useArrayBuffer, onError, onProgress]() -> void {
    std::function<void(const ProgressEvent& event)> progressCallback = nullptr;
    if (onProgress) {
      progressCallback = [onProgress](const ProgressEvent& event) {
        onProgress(SceneLoaderProgressEvent::FromProgressEvent(event));
      };
    }

    FileTools::LoadFile(fileInfo.url, dataCallback, progressCallback, useArrayBuffer,
                        [onError](const std::string& message, const std::string& exception) {
                          onError("Failed to load scene." + (message.empty() ? "" : " " + message),
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

std::shared_ptr<IFileInfo> SceneLoader::_GetFileInfo(std::string rootUrl,
                                                     const std::string& sceneFilename)
{
  std::string url;
  std::string name;

  if (sceneFilename.empty()) {
    url     = rootUrl;
    name    = Tools::GetFilename(rootUrl);
    rootUrl = Tools::GetFolderPath(rootUrl);
  }
  else if (Tools::IsBase64(sceneFilename)) {
    url  = rootUrl + sceneFilename;
    name = "";
  }
  else if (StringTools::startsWith(sceneFilename, "data:")) {
    url  = sceneFilename;
    name = "";
  }
  else {
    if (sceneFilename.substr(0, 1) == "/") {
      BABYLON_LOG_ERROR("SceneLoader", "Wrong sceneFilename parameter")
      return nullptr;
    }

    url  = rootUrl + sceneFilename;
    name = sceneFilename;
  }

  auto r     = std::make_shared<IFileInfo>();
  r->url     = url;
  r->rootUrl = rootUrl;
  r->name    = name;
  r->file    = std::nullopt;
  return r;
}

std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr, ISceneLoaderPluginFactoryPtr>
SceneLoader::GetPluginForExtension(const std::string& extension)
{
  return SceneLoader::_GetPluginForExtension(extension).plugin;
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

  if (!syncedPlugin && !asyncedPlugin) {
    return;
  }

  auto extensions = syncedPlugin ? syncedPlugin->extensions : asyncedPlugin->extensions;
  if (std::holds_alternative<std::string>(extensions)) {
    auto extension = std::get<std::string>(extensions);
    SceneLoader::_registeredPlugins[StringTools::toLowerCase(extension)] = {
      plugin, // plugin
      false   // isBinary
    };
  }
  else {
    auto extensionList = std::get<ISceneLoaderPluginExtensions>(extensions);
    for (auto& item : extensionList.mapping) {
      SceneLoader::_registeredPlugins[StringTools::toLowerCase(item.first)] = {
        plugin,     // plugin
        item.second // isBinary
      };
    }
  }
}

std::optional<std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>>
SceneLoader::ImportMesh(
  const std::vector<std::string>& meshNames, std::string rootUrl, std::string sceneFilename,
  Scene* scene, const SceneLoaderSuccessCallback& onSuccess,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::function<void(Scene* scene, const std::string& message, const std::string& exception)>&
    onError,
  const std::string& pluginExtension)
{
  scene = scene ? scene : Engine::LastCreatedScene();

  if (!scene) {
    BABYLON_LOG_ERROR("SceneLoader", "No scene available to import mesh to")
    return std::nullopt;
  }

  std::shared_ptr<IFileInfo> fileInfo = SceneLoader::_GetFileInfo(rootUrl, sceneFilename);
  if (!fileInfo) {
    return std::nullopt;
  }

  const auto disposeHandler = []() {};

  const auto errorHandler = [=](const std::string& message, const std::string& exception) {
    const auto errorMessage = StringTools::concat("Unable to import meshes from ", fileInfo->url,
                                                  (!message.empty() ? ": " + message : ""));

    if (onError) {
      onError(scene, errorMessage, exception);
    }
    else {
      BABYLON_LOG_ERROR("SceneLoader", errorMessage)
      // should the exception be thrown?
    }

    disposeHandler();
  };

  std::function<void(const SceneLoaderProgressEvent& event)> progressHandler = nullptr;
  if (onProgress) {
    progressHandler = [=](const SceneLoaderProgressEvent& event) {
      try {
        onProgress(event);
      }
      catch (const std::exception& e) {
        errorHandler(StringTools::printf("Error in onProgress callback: %s", e.what()), e.what());
      }
    };
  }

  const auto successHandler
    = [=](const std::vector<AbstractMeshPtr>& meshes,
          const std::vector<IParticleSystemPtr>& particleSystems,
          const std::vector<SkeletonPtr>& skeletons,
          const std::vector<AnimationGroupPtr>& animationGroups,
          const std::vector<TransformNodePtr>& transformNodes,
          const std::vector<GeometryPtr>& geometries, const std::vector<LightPtr>& lights) {
        scene->importedMeshesFiles.emplace_back(fileInfo->url);

        if (onSuccess) {
          try {
            onSuccess(meshes, particleSystems, skeletons, animationGroups, transformNodes,
                      geometries, lights);
          }
          catch (const std::exception& e) {
            errorHandler(StringTools::printf("Error in onSuccess callback", e.what()), e.what());
          }
        }
      };

  return SceneLoader::_LoadData(
    *fileInfo, scene,
    [=](const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>& plugin,
        const std::string& data, const std::string& responseURL) -> void {
      if (std::holds_alternative<ISceneLoaderPluginPtr>(plugin)) {
        auto syncedPlugin = std::get<ISceneLoaderPluginPtr>(plugin);

        if (!syncedPlugin) {
          return;
        }

        if (syncedPlugin->rewriteRootURL) {
          fileInfo->rootUrl = syncedPlugin->rewriteRootURL(fileInfo->rootUrl, responseURL);
        }

        std::vector<AbstractMeshPtr> meshes;
        std::vector<IParticleSystemPtr> particleSystems;
        std::vector<SkeletonPtr> skeletons;

        if (!syncedPlugin->importMesh(meshNames, scene, data, fileInfo->rootUrl, meshes,
                                      particleSystems, skeletons, errorHandler)) {
          return;
        }

        scene->loadingPluginName = syncedPlugin->name;
        successHandler(meshes, particleSystems, skeletons, {}, {}, {}, {});
      }
      else {
        auto asyncedPlugin = std::get<ISceneLoaderPluginAsyncPtr>(plugin);
        auto result = asyncedPlugin->importMeshAsync(meshNames, scene, data, fileInfo->rootUrl,
                                                     progressHandler, fileInfo->name);
        scene->loadingPluginName = asyncedPlugin->name;
        successHandler(result.meshes, result.particleSystems, result.skeletons,
                       result.animationGroups, result.transformNodes, result.geometries,
                       result.lights);
      }
    },
    progressHandler, errorHandler, disposeHandler, pluginExtension);
}

std::unique_ptr<Scene> SceneLoader::Load(
  const std::string& rootUrl, const std::string& sceneFilename, Engine* engine,
  const std::function<void(Scene* scene)>& onsuccess,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::function<void(Scene* scene, const std::string& message, const std::string& exception)>&
    onError,
  const std::string& pluginExtension)
{
  if (!engine) {
    BABYLON_LOG_ERROR("SceneLoader", "No engine available")
    return nullptr;
  }

  auto scene = Scene::New(engine);
  SceneLoader::Append(rootUrl, sceneFilename, scene.get(), onsuccess, onProgress, onError,
                      pluginExtension);
  return scene;
}

std::optional<std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>> SceneLoader::Append(
  std::string rootUrl, std::string sceneFilename, Scene* scene,
  const std::function<void(Scene* scene)>& onSuccess,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::function<void(Scene* scene, const std::string& message, const std::string& exception)>&
    onError,
  const std::string& pluginExtension)
{
  scene = scene ? scene : Engine::LastCreatedScene();

  if (!scene) {
    BABYLON_LOG_ERROR("SceneLoader", "No scene available to append to")
    return std::nullopt;
  }

  auto fileInfo = SceneLoader::_GetFileInfo(rootUrl, sceneFilename);
  if (!fileInfo) {
    return std::nullopt;
  }

  if (SceneLoader::ShowLoadingScreen()) {
    scene->getEngine()->displayLoadingUI();
  }

  const auto disposeHandler = [&scene]() { scene->getEngine()->hideLoadingUI(); };

  const auto errorHandler = [=](const std::string& message, const std::string& exception) {
    const auto errorMessage = StringTools::concat("Unable to load from ", fileInfo->url,
                                                  (!message.empty() ? ": " + message : ""));
    if (onError) {
      onError(scene, errorMessage, exception);
    }
    else {
      BABYLON_LOG_ERROR("SceneLoader", errorMessage)
      // should the exception be thrown?
    }

    disposeHandler();
  };

  std::function<void(const SceneLoaderProgressEvent& event)> progressHandler = nullptr;
  if (onProgress) {
    progressHandler = [=](const SceneLoaderProgressEvent& event) {
      try {
        onProgress(event);
      }
      catch (const std::exception& e) {
        errorHandler("Error in onProgress callback", e.what());
      }
    };
  }

  const auto successHandler = [=]() {
    if (onSuccess) {
      try {
        onSuccess(scene);
      }
      catch (const std::exception& e) {
        errorHandler("Error in onSuccess callback", e.what());
      }
    }
  };

  return SceneLoader::_LoadData(
    *fileInfo, scene,
    [=](const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>& plugin,
        const std::string& data, const std::string& /*responseURL*/) -> void {
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
        asyncedPlugin->loadAsync(scene, data, fileInfo->rootUrl, progressHandler, fileInfo->name);
        scene->loadingPluginName = asyncedPlugin->name;
        successHandler();
      }

      if (SceneLoader::ShowLoadingScreen()) {
        scene->executeWhenReady(
          [](Scene* scene, EventState& /*es*/) { scene->getEngine()->hideLoadingUI(); });
      }
    },
    progressHandler, errorHandler, disposeHandler, pluginExtension);
}

} // end of namespace BABYLON
