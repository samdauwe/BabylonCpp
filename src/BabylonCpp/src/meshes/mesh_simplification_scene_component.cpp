#include <babylon/meshes/mesh_simplification_scene_component.h>

#include <babylon/engines/scene.h>
#include <babylon/meshes/simplification/simplification_queue.h>

namespace BABYLON {

SimplicationQueueSceneComponent::SimplicationQueueSceneComponent(Scene* iScene)
{
  ISceneComponent::name = SimplicationQueueSceneComponent::name;
  scene                 = iScene;
}

SimplicationQueueSceneComponent::~SimplicationQueueSceneComponent() = default;

void SimplicationQueueSceneComponent::_register()
{
  scene->_beforeCameraUpdateStage.registerStep(
    SceneComponentConstants::STEP_BEFORECAMERAUPDATE_SIMPLIFICATIONQUEUE, this,
    [this]() { _beforeCameraUpdate(); });
}

void SimplicationQueueSceneComponent::rebuild()
{
  // Nothing to do for this component
}

void SimplicationQueueSceneComponent::dispose()
{
  // Nothing to do for this component
}

void SimplicationQueueSceneComponent::_beforeCameraUpdate()
{
  if (scene->simplificationQueue() && !scene->simplificationQueue()->running) {
    scene->simplificationQueue()->executeNext();
  }
}

} // end of namespace BABYLON
