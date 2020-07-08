#include <babylon/loading/scene_loader_flags.h>

#include <babylon/engines/constants.h>

namespace BABYLON {

// Flags
bool SceneLoaderFlags::_ForceFullSceneLoadingForIncremental = false;
bool SceneLoaderFlags::_ShowLoadingScreen                   = true;
bool SceneLoaderFlags::_CleanBoneMatrixWeights              = false;
unsigned int SceneLoaderFlags::_loggingLevel                = Constants::SCENELOADER_NO_LOGGING;

bool SceneLoaderFlags::ForceFullSceneLoadingForIncremental()
{
  return SceneLoaderFlags::_ForceFullSceneLoadingForIncremental;
}

void SceneLoaderFlags::setForceFullSceneLoadingForIncremental(bool value)
{
  SceneLoaderFlags::_ForceFullSceneLoadingForIncremental = value;
}

bool SceneLoaderFlags::ShowLoadingScreen()
{
  return SceneLoaderFlags::_ShowLoadingScreen;
}

void SceneLoaderFlags::setShowLoadingScreen(bool value)
{
  SceneLoaderFlags::_ShowLoadingScreen = value;
}

unsigned int SceneLoaderFlags::loggingLevel()
{
  return SceneLoaderFlags::_loggingLevel;
}

void SceneLoaderFlags::setloggingLevel(unsigned int value)
{
  SceneLoaderFlags::_loggingLevel = value;
}

bool SceneLoaderFlags::CleanBoneMatrixWeights()
{
  return SceneLoaderFlags::_CleanBoneMatrixWeights;
}

void SceneLoaderFlags::setCleanBoneMatrixWeights(bool value)
{
  SceneLoaderFlags::_CleanBoneMatrixWeights = value;
}

} // end of namespace BABYLON
