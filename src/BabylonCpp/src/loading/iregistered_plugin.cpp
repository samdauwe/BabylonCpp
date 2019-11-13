#include <babylon/loading/iregistered_plugin.h>

#include <babylon/loading/iscene_loader_plugin.h>
#include <babylon/loading/iscene_loader_plugin_async.h>

namespace BABYLON {

IRegisteredPlugin::IRegisteredPlugin() : isBinary{false}
{
}

IRegisteredPlugin::IRegisteredPlugin(
  const std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>&
    iPlugin,
  bool iIsBinary)
    : isBinary{iIsBinary}
{
  if (std::holds_alternative<ISceneLoaderPluginPtr>(iPlugin)) {
    plugin = std::get<ISceneLoaderPluginPtr>(iPlugin);
  }
  else {
    plugin = std::get<ISceneLoaderPluginAsyncPtr>(iPlugin);
  }
}

IRegisteredPlugin::IRegisteredPlugin(const IRegisteredPlugin& other) = default;

IRegisteredPlugin::IRegisteredPlugin(IRegisteredPlugin&& other) = default;

IRegisteredPlugin& IRegisteredPlugin::operator=(const IRegisteredPlugin& other) = default;

IRegisteredPlugin& IRegisteredPlugin::operator=(IRegisteredPlugin&& other) =  default;

IRegisteredPlugin::~IRegisteredPlugin() = default;

} // end of namespace BABYLON
