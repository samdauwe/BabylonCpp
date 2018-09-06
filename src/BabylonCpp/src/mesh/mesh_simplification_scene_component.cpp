#include <babylon/mesh/mesh_simplification_scene_component.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/simplification/simplification_queue.h>

namespace BABYLON {

SimplicationQueueSceneComponent::SimplicationQueueSceneComponent(Scene* iScene)
{
  scene = iScene;
}

SimplicationQueueSceneComponent::~SimplicationQueueSceneComponent()
{
}

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
