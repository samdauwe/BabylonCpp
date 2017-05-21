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

bool SceneLoader::ForceFullSceneLoadingForIncremental = false;
bool SceneLoader::ShowLoadingScreen                   = true;
unsigned int SceneLoader::LoggingLevel                = SceneLoader::NO_LOGGING;

std::unordered_map<std::string, IRegisteredPlugin>
  SceneLoader::_registeredPlugins{};

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

  return SceneLoader::_getDefaultPlugin();
}

IRegisteredPlugin
SceneLoader::_getPluginForFilename(const std::string& sceneFilename)
{
  auto dotPosition = sceneFilename.find_last_of(".");

  auto queryStringPosition = sceneFilename.find("?", 0);

  if (queryStringPosition == std::string::npos) {
    queryStringPosition = sceneFilename.size();
  }

  auto extension = String::toLowerCase(
    sceneFilename.substr(dotPosition, queryStringPosition));
  return SceneLoader::_getPluginForExtension(extension);
}

std::string SceneLoader::_getDirectLoad(const std::string& sceneFilename)
{
  if (String::startsWith(sceneFilename, "data:")) {
    return sceneFilename.substr(5);
  }

  return "";
}

ISceneLoaderPlugin*
SceneLoader::GetPluginForExtension(const std::string& extension)
{
  return SceneLoader::_getPluginForExtension(extension).plugin.get();
}

void SceneLoader::RegisterPlugin(std::shared_ptr<ISceneLoaderPlugin>&& plugin)
{
  auto& extensions = plugin->extensions.mapping;
  for (auto& item : extensions) {
    SceneLoader::_registeredPlugins[String::toLowerCase(item.first)] = {
      std::move(plugin), // plugin
      item.second        // isBinary
    };
  }
}

void SceneLoader::ImportMesh(
  const std::vector<std::string>& meshesNames, const std::string& rootUrl,
  const std::string& sceneFilename, Scene* scene,
  const std::function<void(std::vector<AbstractMesh*>& meshes,
                           std::vector<ParticleSystem*>& particleSystems,
                           std::vector<Skeleton*>& skeletons)>& onsuccess,
  const std::function<void()>& progressCallBack,
  const std::function<void(Scene* scene, const std::string& message,
                           const std::string& exception)>& onerror)
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

  auto importMeshFromData = [&](const std::string& data) {
    std::vector<AbstractMesh*> meshes;
    std::vector<ParticleSystem*> particleSystems;
    std::vector<Skeleton*> skeletons;

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
                  [&](const std::string& data) { importMeshFromData(data); },
                  progressCallBack, useArrayBuffer);
}

std::unique_ptr<Scene>
SceneLoader::Load(const std::string& rootUrl, const std::string& sceneFilename,
                  Engine* engine,
                  const std::function<void(Scene* scene)>& onsuccess,
                  const std::function<void()>& progressCallBack,
                  const std::function<void(Scene* scene)>& onerror)
{
  auto scene = Scene::New(engine);
  SceneLoader::Append(rootUrl, sceneFilename, scene.get(), onsuccess,
                      progressCallBack, onerror);
  return scene;
}

void SceneLoader::Append(const std::string& rootUrl,
                         const std::string& sceneFilename, Scene* scene,
                         const std::function<void(Scene* scene)>& onsuccess,
                         const std::function<void()>& progressCallBack,
                         const std::function<void(Scene* scene)>& onerror)
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

  if (SceneLoader::ShowLoadingScreen) {
    scene->getEngine()->displayLoadingUI();
  }

  auto loadSceneFromData = [&](const std::string& data) {
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

    if (SceneLoader::ShowLoadingScreen) {
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
