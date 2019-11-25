#include <babylon/engines/engine_store.h>

namespace BABYLON {

std::vector<Engine*> EngineStore::Instances;

Scene* EngineStore::_LastCreatedScene = nullptr;

Engine* EngineStore::LastCreatedEngine()
{
  if (Instances.empty()) {
    return nullptr;
  }

  return Instances.back();
}

Scene* EngineStore::LastCreatedScene()
{
  return _LastCreatedScene;
}

bool EngineStore::UseFallbackTexture = true;

std::string EngineStore::FallbackTexture = "";

} // end of namespace BABYLON
