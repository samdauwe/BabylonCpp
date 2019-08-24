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

} // end of namespace BABYLON
