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

IRegisteredPlugin::IRegisteredPlugin(const IRegisteredPlugin& other)
    : plugin{other.plugin}, isBinary{other.isBinary}
{
}

IRegisteredPlugin::IRegisteredPlugin(IRegisteredPlugin&& other)
    : plugin{std::move(other.plugin)}, isBinary{std::move(other.isBinary)}
{
}

IRegisteredPlugin& IRegisteredPlugin::operator=(const IRegisteredPlugin& other)
{
  if (&other != this) {
    plugin   = other.plugin;
    isBinary = other.isBinary;
  }

  return *this;
}

IRegisteredPlugin& IRegisteredPlugin::operator=(IRegisteredPlugin&& other)
{
  if (&other != this) {
    plugin   = std::move(other.plugin);
    isBinary = std::move(other.isBinary);
  }

  return *this;
}

IRegisteredPlugin::~IRegisteredPlugin()
{
}

} // end of namespace BABYLON
