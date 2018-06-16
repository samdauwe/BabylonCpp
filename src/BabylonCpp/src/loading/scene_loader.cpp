#include <babylon/loading/scene_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/loading/iregistered_plugin.h>
#include <babylon/loading/plugins/babylon/babylon_file_loader.h>
#include <babylon/loading/scene_loader_progress_event.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

bool SceneLoader::_ForceFullSceneLoadingForIncremental = false;
bool SceneLoader::_ShowLoadingScreen                   = true;
bool SceneLoader::_CleanBoneMatrixWeights              = false;
unsigned int SceneLoader::_loggingLevel = SceneLoader::NO_LOGGING();
Observable<ISceneLoaderPlugin> SceneLoader::OnPluginActivatedObservable;

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

unordered_map_t<string_t, IRegisteredPlugin> SceneLoader::_registeredPlugins{};

IRegisteredPlugin SceneLoader::_getDefaultPlugin()
{
  // Add default plugin
  if (SceneLoader::_registeredPlugins.empty()) {
    SceneLoader::RegisterPlugin(::std::make_shared<BabylonFileLoader>());
  }

  return SceneLoader::_registeredPlugins[".babylon"];
}

IRegisteredPlugin SceneLoader::_getPluginForExtension(const string_t& extension)
{
  if (stl_util::contains(SceneLoader::_registeredPlugins, extension)) {
    return SceneLoader::_registeredPlugins[extension];
  }

  BABYLON_LOGF_WARN(
    "SceneLoader",
    "Unable to find a plugin to load %s files. Trying to use .babylon default "
    "plugin. To load from a specific filetype (eg. gltf) see: "
    "http://doc.babylonjs.com/how_to/load_from_any_file_type",
    extension.c_str());
  return SceneLoader::_getDefaultPlugin();
}

IRegisteredPlugin SceneLoader::_getPluginForDirectLoad(const string_t& data)
{
  for (const auto& pluginItem : SceneLoader::_registeredPlugins) {
    const auto& registeredPlugin = pluginItem.second;

    if (registeredPlugin.plugin->canDirectLoad
        && registeredPlugin.plugin->canDirectLoad(data)) {
      return registeredPlugin;
    }
  }

  return SceneLoader::_getDefaultPlugin();
}

IRegisteredPlugin SceneLoader::_getPluginForFilename(string_t sceneFilename)
{
  auto queryStringPosition = sceneFilename.find("?", 0);

  if (queryStringPosition == string_t::npos) {
    sceneFilename = sceneFilename.substr(0, queryStringPosition);
  }

  auto dotPosition = sceneFilename.find_last_of(".");

  auto extension = String::toLowerCase(
    sceneFilename.substr(dotPosition, sceneFilename.size()));
  return SceneLoader::_getPluginForExtension(extension);
}

string_t SceneLoader::_getDirectLoad(const string_t& sceneFilename)
{
  if (String::startsWith(sceneFilename, "data:")) {
    return sceneFilename.substr(5);
  }

  return "";
}

shared_ptr_t<ISceneLoaderPlugin> SceneLoader::_loadData(
  const string_t& rootUrl, const string_t& sceneFilename, Scene* scene,
  const ::std::function<void(const shared_ptr_t<ISceneLoaderPlugin>& plugin,
                             const string_t& data,
                             const string_t& responseURL)>& onSuccess,
  const ::std::function<void(const SceneLoaderProgressEvent& event)>&
    onProgress,
  const ::std::function<void(const string_t& message,
                             const string_t& exception)>& onError,
  const ::std::function<void()>& /*onDispose*/, const string_t& pluginExtension)
{
  auto directLoad = SceneLoader::_getDirectLoad(sceneFilename);
  auto registeredPlugin
    = (!pluginExtension.empty()) ?
        SceneLoader::_getPluginForExtension(pluginExtension) :
        (!directLoad.empty() ?
           SceneLoader::_getPluginForDirectLoad(sceneFilename) :
           SceneLoader::_getPluginForFilename(sceneFilename));
  auto& plugin         = registeredPlugin.plugin;
  auto& useArrayBuffer = registeredPlugin.isBinary;

  SceneLoader::OnPluginActivatedObservable.notifyObservers(plugin.get());

  const auto dataCallback
    = [&](const string_t& data, const string_t& responseURL) {
        if (scene->isDisposed()) {
          onError("Scene has been disposed", "");
          return;
        }

        onSuccess(plugin, data, responseURL);
      };

  const auto manifestChecked = [&]() {
    auto url = rootUrl + sceneFilename;
    ::std::function<void(const ProgressEvent& event)> progressCallback
      = nullptr;
    if (onProgress) {
      progressCallback = [&](const ProgressEvent& event) {
        onProgress(SceneLoaderProgressEvent::FromProgressEvent(event));
      };
    }

    Tools::LoadFile(url, dataCallback, progressCallback, useArrayBuffer,
                    [&](const string_t& exception) {
                      onError("Failed to load scene."
                                + (exception.empty() ? "" : " " + exception),
                              exception);
                    });
  };

  if (!directLoad.empty()) {
    // Direct load
    dataCallback(directLoad, "");
    return plugin;
  }

  // Loading file from disk
  if (!sceneFilename.empty()) {
    manifestChecked();
  }
  else {
    onError("Unable to find file named " + sceneFilename, "");
  }

  return plugin;
}

ISceneLoaderPlugin*
SceneLoader::GetPluginForExtension(const string_t& extension)
{
  return SceneLoader::_getPluginForExtension(extension).plugin.get();
}

bool SceneLoader::IsPluginForExtensionAvailable(const string_t& extension)
{
  return stl_util::contains(SceneLoader::_registeredPlugins, extension);
}

void SceneLoader::RegisterPlugin(shared_ptr_t<ISceneLoaderPlugin>&& plugin)
{
  auto& extensions = plugin->extensions.mapping;
  for (auto& item : extensions) {
    SceneLoader::_registeredPlugins[String::toLowerCase(item.first)] = {
      ::std::move(plugin), // plugin
      item.second          // isBinary
    };
  }
}

shared_ptr_t<ISceneLoaderPlugin> SceneLoader::ImportMesh(
  const vector_t<string_t>& meshNames, string_t rootUrl, string_t sceneFilename,
  Scene* scene,
  const ::std::function<void(const vector_t<AbstractMesh*>& meshes,
                             const vector_t<ParticleSystem*>& particleSystems,
                             const vector_t<Skeleton*>& skeletons,
                             const vector_t<AnimationGroup*>& animationGroups)>&
    onSuccess,
  const ::std::function<void(const SceneLoaderProgressEvent& event)>&
    onProgress,
  const ::std::function<void(Scene* scene, const string_t& message,
                             const string_t& exception)>& onError,
  const string_t& pluginExtension)
{
  scene = scene ? scene : Engine::LastCreatedScene();

  if (!scene) {
    BABYLON_LOG_ERROR("SceneLoader", "No scene available to import mesh to");
    return nullptr;
  }

  if (sceneFilename.empty()) {
    sceneFilename = Tools::GetFilename(rootUrl);
    rootUrl       = Tools::GetFolderPath(rootUrl);
  }

  if (String::startsWith(sceneFilename, "/")) {
    BABYLON_LOG_ERROR("SceneLoader", "Wrong sceneFilename parameter");
    return nullptr;
  }

  const auto disposeHandler = []() {};

  const auto errorHandler = [&](const string_t& message,
                                const string_t& exception) {
    const auto errorMessage
      = String::concat("Unable to import meshes from ", rootUrl, sceneFilename,
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

  ::std::function<void(const SceneLoaderProgressEvent& event)> progressHandler
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
    = [&](const vector_t<AbstractMesh*>& meshes,
          const vector_t<ParticleSystem*>& particleSystems,
          const vector_t<Skeleton*>& skeletons,
          const vector_t<AnimationGroup*>& animationGroups) {
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
    rootUrl, sceneFilename, scene,
    [&](const shared_ptr_t<ISceneLoaderPlugin>& plugin, const string_t& data,
        const string_t& responseURL) {
      if (plugin->rewriteRootURL) {
        rootUrl = plugin->rewriteRootURL(rootUrl, responseURL);
      }

      if (sceneFilename == "") {
        if (sceneFilename == "") {
          rootUrl = Tools::GetFolderPath(rootUrl, true);
        }
      }

      auto& syncedPlugin = plugin;
      vector_t<AbstractMesh*> meshes;
      vector_t<ParticleSystem*> particleSystems;
      vector_t<Skeleton*> skeletons;
      vector_t<AnimationGroup*> animationGroups;

      if (!syncedPlugin->importMesh(meshNames, scene, data, rootUrl, meshes,
                                    particleSystems, skeletons, errorHandler)) {
        return;
      }

      scene->loadingPluginName = plugin->name;
      successHandler(meshes, particleSystems, skeletons, animationGroups);
    },
    progressHandler, errorHandler, disposeHandler, pluginExtension);
}

unique_ptr_t<Scene> SceneLoader::Load(
  const string_t& rootUrl, const string_t& sceneFilename, Engine* engine,
  const ::std::function<void(Scene* scene)>& onsuccess,
  const ::std::function<void(const SceneLoaderProgressEvent& event)>&
    onProgress,
  const ::std::function<void(Scene* scene, const string_t& message,
                             const string_t& exception)>& onError,
  const string_t& pluginExtension)
{
  auto scene = Scene::New(engine);
  SceneLoader::Append(rootUrl, sceneFilename, scene.get(), onsuccess,
                      onProgress, onError, pluginExtension);
  return scene;
}

shared_ptr_t<ISceneLoaderPlugin> SceneLoader::Append(
  string_t rootUrl, string_t sceneFilename, Scene* scene,
  const ::std::function<void(Scene* scene)>& onSuccess,
  const ::std::function<void(const SceneLoaderProgressEvent& event)>&
    onProgress,
  const ::std::function<void(Scene* scene, const string_t& message,
                             const string_t& exception)>& onError,
  const string_t& pluginExtension)
{
  scene = scene ? scene : Engine::LastCreatedScene();

  if (!scene) {
    BABYLON_LOG_ERROR("SceneLoader", "No scene available to append to");
    return nullptr;
  }

  if (sceneFilename.empty()) {
    sceneFilename = Tools::GetFilename(rootUrl);
    rootUrl       = Tools::GetFolderPath(rootUrl);
  }

  if (String::startsWith(sceneFilename, "/")) {
    BABYLON_LOG_ERROR("SceneLoader", "Wrong sceneFilename parameter");
    return nullptr;
  }

  if (SceneLoader::ShowLoadingScreen()) {
    scene->getEngine()->displayLoadingUI();
  }

  const auto disposeHandler
    = [&scene]() { scene->getEngine()->hideLoadingUI(); };

  const auto errorHandler
    = [&](const string_t& message, const string_t& exception) {
        const auto errorMessage
          = String::concat("Unable to load from ", rootUrl, sceneFilename,
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

  ::std::function<void(const SceneLoaderProgressEvent& event)> progressHandler
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
    rootUrl, sceneFilename, scene,
    [&](const shared_ptr_t<ISceneLoaderPlugin>& plugin, const string_t& data,
        const string_t& /*responseURL*/) {
      if (sceneFilename == "") {
        rootUrl = Tools::GetFolderPath(rootUrl, true);
      }

      auto& syncedPlugin = plugin;
      if (!syncedPlugin->load(scene, data, rootUrl, errorHandler)) {
        return;
      }

      scene->loadingPluginName = plugin->name;
      successHandler();

      if (SceneLoader::ShowLoadingScreen()) {
        scene->executeWhenReady([](Scene* scene, EventState& /*es*/) {
          scene->getEngine()->hideLoadingUI();
        });
      }
    },
    progressHandler, errorHandler, disposeHandler, pluginExtension);
}

} // end of namespace BABYLON
