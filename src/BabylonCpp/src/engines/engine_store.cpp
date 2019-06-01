#include <babylon/engines/engine_store.h>

namespace BABYLON {

std::vector<EnginePtr> EngineStore::Instances;

ScenePtr EngineStore::_LastCreatedScene = nullptr;

EnginePtr EngineStore::LastCreatedEngine()
{
  if (Instances.empty()) {
    return nullptr;
  }

  return Instances.back();
}

ScenePtr EngineStore::LastCreatedScene()
{
  return _LastCreatedScene;
}

} // end of namespace BABYLON
