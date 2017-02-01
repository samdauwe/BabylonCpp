#include <babylon/loading/scene_loader.h>

#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/loading/iregistered_plugin.h>

namespace BABYLON {

bool SceneLoader::ForceFullSceneLoadingForIncremental = false;
bool SceneLoader::ShowLoadingScreen                   = true;
unsigned int SceneLoader::LoggingLevel                = SceneLoader::NO_LOGGING;

std::unordered_map<std::string, IRegisteredPlugin>
  SceneLoader::_registeredPlugins{};

IRegisteredPlugin SceneLoader::_getDefaultPlugin()
{
  return SceneLoader::_registeredPlugins[".babylon"];
}

IRegisteredPlugin
SceneLoader::_getPluginForExtension(const std::string& extension)
{
  if (std_util::contains(SceneLoader::_registeredPlugins, extension)) {
    return SceneLoader::_registeredPlugins[extension];
  }

  return SceneLoader::_getDefaultPlugin();
}

ISceneLoaderPlugin*
SceneLoader::_getPluginForFilename(const std::string& sceneFilename)
{
  auto dotPosition = sceneFilename.find_last_of(".");

  auto queryStringPosition = sceneFilename.find("?", 0);

  if (queryStringPosition == std::string::npos) {
    queryStringPosition = sceneFilename.size();
  }

  auto extension = String::toLowerCase(
    sceneFilename.substr(dotPosition, queryStringPosition));
  return SceneLoader::GetPluginForExtension(extension);
}

std::string SceneLoader::_getDirectLoad(const std::string& sceneFilename)
{
  if ((sceneFilename.size() > 5) && sceneFilename.substr(0, 5) == "data:") {
    return sceneFilename.substr(5);
  }

  return "";
}

ISceneLoaderPlugin*
SceneLoader::GetPluginForExtension(const std::string& extension)
{
  return SceneLoader::_getPluginForExtension(extension).plugin;
}

void SceneLoader::RegisterPlugin(ISceneLoaderPlugin* plugin)
{
  auto& extensions = plugin->extensions.mapping;
  for (auto& item : extensions) {
    SceneLoader::_registeredPlugins[String::toLowerCase(item.first)] = {
      plugin,     // plugin
      item.second // isBinary
    };
  }
}

void SceneLoader::ImportMesh(
  const std::vector<std::string>& /*meshesNames*/,
  const std::string& /*rootUrl*/, const std::string& /*sceneFilename*/,
  Scene* /*scene*/,
  const std::function<void(std::vector<AbstractMesh*>& meshes,
                           std::vector<ParticleSystem*>& particleSystems,
                           std::vector<Skeleton*>& skeletons)>& /*onsuccess*/,
  const std::function<void()>& /*progressCallBack*/,
  const std::function<void(Scene* scene, const std::string& message,
                           const std::string& exception)>& /*onerror*/)
{
}

void SceneLoader::Load(const std::string& /*rootUrl*/,
                       const std::string& /*sceneFilename*/, Engine* /*engine*/,
                       const std::function<void(Scene* scene)>& /*onsuccess*/,
                       const std::function<void()>& /*progressCallBack*/,
                       const std::function<void(Scene* scene)>& /*onerror*/)
{
  // SceneLoader::Append(rootUrl, sceneFilename, new Scene(engine), onsuccess,
  //                    progressCallBack, onerror);
}

void SceneLoader::Append(const std::string& /*rootUrl*/,
                         const std::string& /*sceneFilename*/, Scene* /*scene*/,
                         const std::function<void(Scene* scene)>& /*onsuccess*/,
                         const std::function<void()>& /*progressCallBack*/,
                         const std::function<void(Scene* scene)>& /*onerror*/)
{
}

} // end of namespace BABYLON
