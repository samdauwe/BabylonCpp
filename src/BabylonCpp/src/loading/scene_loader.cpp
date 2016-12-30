#include <babylon/loading/scene_loader.h>

#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

bool SceneLoader::ForceFullSceneLoadingForIncremental = false;
bool SceneLoader::ShowLoadingScreen                   = true;
unsigned int SceneLoader::LoggingLevel                = SceneLoader::NO_LOGGING;

std::vector<ISceneLoaderPlugin*> SceneLoader::RegisteredPlugins{};

ISceneLoaderPlugin*
SceneLoader::_getPluginForFilename(const std::string& sceneFilename)
{
  size_t dotPosition = sceneFilename.find_last_of(".");

  size_t queryStringPosition = sceneFilename.find("?", 0);

  if (queryStringPosition == std::string::npos) {
    queryStringPosition = sceneFilename.size();
  }

  std::string extension = sceneFilename.substr(dotPosition + 1);

  return SceneLoader::GetPluginForExtension(extension);
}

ISceneLoaderPlugin*
SceneLoader::GetPluginForExtension(const std::string& extension)
{
  auto it = std::find_if(RegisteredPlugins.begin(), RegisteredPlugins.end(),
                         [&extension](const ISceneLoaderPlugin* plugin) {
                           return plugin->extensions.find(extension)
                                  != std::string::npos;
                         });

  return (it != RegisteredPlugins.end()) ? *it : nullptr;
}

void SceneLoader::RegisterPlugin(ISceneLoaderPlugin* plugin)
{
  plugin->extensions = String::toLowerCase(plugin->extensions);
  SceneLoader::RegisteredPlugins.emplace_back(plugin);
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
