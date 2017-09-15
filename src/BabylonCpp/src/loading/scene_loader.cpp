#include <babylon/loading/scene_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/loading/iregistered_plugin.h>
#include <babylon/loading/plugins/babylon/babylon_file_loader.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

bool SceneLoader::_ForceFullSceneLoadingForIncremental = false;
bool SceneLoader::_ShowLoadingScreen                   = true;
bool SceneLoader::_CleanBoneMatrixWeights              = false;
unsigned int SceneLoader::_loggingLevel = SceneLoader::NO_LOGGING;

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

  return SceneLoader::_getDefaultPlugin();
}

IRegisteredPlugin
SceneLoader::_getPluginForFilename(const string_t& sceneFilename)
{
  auto dotPosition = sceneFilename.find_last_of(".");

  auto queryStringPosition = sceneFilename.find("?", 0);

  if (queryStringPosition == string_t::npos) {
    queryStringPosition = sceneFilename.size();
  }

  auto extension = String::toLowerCase(
    sceneFilename.substr(dotPosition, queryStringPosition));
  return SceneLoader::_getPluginForExtension(extension);
}

string_t SceneLoader::_getDirectLoad(const string_t& sceneFilename)
{
  if (String::startsWith(sceneFilename, "data:")) {
    return sceneFilename.substr(5);
  }

  return "";
}

ISceneLoaderPlugin*
SceneLoader::GetPluginForExtension(const string_t& extension)
{
  return SceneLoader::_getPluginForExtension(extension).plugin.get();
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

void SceneLoader::ImportMesh(
  const vector_t<string_t>& meshesNames, const string_t& rootUrl,
  const string_t& sceneFilename, Scene* scene,
  const ::std::function<void(vector_t<AbstractMesh*>& meshes,
                             vector_t<ParticleSystem*>& particleSystems,
                             vector_t<Skeleton*>& skeletons)>& onsuccess,
  const ::std::function<void()>& progressCallBack,
  const ::std::function<void(Scene* scene, const string_t& message,
                             const string_t& exception)>& onerror)
{
  if (String::startsWith(sceneFilename, "/")) {
    BABYLON_LOG_ERROR("SceneLoader", "Wrong sceneFilename parameter");
    return;
  }

  auto directLoad       = SceneLoader::_getDirectLoad(sceneFilename);
  auto registeredPlugin = (!directLoad.empty()) ?
                            SceneLoader::_getDefaultPlugin() :
                            SceneLoader::_getPluginForFilename(sceneFilename);
  auto& plugin         = registeredPlugin.plugin;
  auto& useArrayBuffer = registeredPlugin.isBinary;

  auto importMeshFromData = [&](const string_t& data) {
    vector_t<AbstractMesh*> meshes;
    vector_t<ParticleSystem*> particleSystems;
    vector_t<Skeleton*> skeletons;

    if (!plugin->importMesh(meshesNames, scene, data, rootUrl, meshes,
                            particleSystems, skeletons)) {
      if (onerror) {
        onerror(scene,
                "Unable to import meshes from " + rootUrl + sceneFilename, "");
      }
      return;
    }

    if (onsuccess) {
      scene->importedMeshesFiles.emplace_back(rootUrl + sceneFilename);
      onsuccess(meshes, particleSystems, skeletons);
    }
  };

  if (!directLoad.empty()) {
    importMeshFromData(directLoad);
    return;
  }

  Tools::LoadFile(rootUrl + sceneFilename,
                  [&](const string_t& data) { importMeshFromData(data); },
                  progressCallBack, useArrayBuffer);
}

unique_ptr_t<Scene>
SceneLoader::Load(const string_t& rootUrl, const string_t& sceneFilename,
                  Engine* engine,
                  const ::std::function<void(Scene* scene)>& onsuccess,
                  const ::std::function<void()>& progressCallBack,
                  const ::std::function<void(Scene* scene)>& onerror)
{
  auto scene = Scene::New(engine);
  SceneLoader::Append(rootUrl, sceneFilename, scene.get(), onsuccess,
                      progressCallBack, onerror);
  return scene;
}

void SceneLoader::Append(const string_t& rootUrl, const string_t& sceneFilename,
                         Scene* scene,
                         const ::std::function<void(Scene* scene)>& onsuccess,
                         const ::std::function<void()>& progressCallBack,
                         const ::std::function<void(Scene* scene)>& onerror)
{
  if (String::startsWith(sceneFilename, "/")) {
    BABYLON_LOG_ERROR("SceneLoader", "Wrong sceneFilename parameter");
    return;
  }

  auto directLoad       = SceneLoader::_getDirectLoad(sceneFilename);
  auto registeredPlugin = (!directLoad.empty()) ?
                            SceneLoader::_getDefaultPlugin() :
                            SceneLoader::_getPluginForFilename(sceneFilename);
  auto& plugin         = registeredPlugin.plugin;
  auto& useArrayBuffer = registeredPlugin.isBinary;

  if (SceneLoader::ShowLoadingScreen()) {
    scene->getEngine()->displayLoadingUI();
  }

  auto loadSceneFromData = [&](const string_t& data) {
    if (!plugin->load(scene, data, rootUrl)) {
      if (onerror) {
        onerror(scene);
      }
      scene->getEngine()->hideLoadingUI();
      return;
    }

    if (onsuccess) {
      onsuccess(scene);
    }

    if (SceneLoader::ShowLoadingScreen()) {
      scene->executeWhenReady([&]() { scene->getEngine()->hideLoadingUI(); });
    }
  };

  if (!directLoad.empty()) {
    // Direct load
    loadSceneFromData(directLoad);
    return;
  }

  // Loading file from disk via input file or drag'n'drop
  if (String::startsWith(rootUrl, "file:")) {
    Tools::LoadFile(sceneFilename, loadSceneFromData, progressCallBack,
                    useArrayBuffer);
  }
}

} // end of namespace BABYLON
