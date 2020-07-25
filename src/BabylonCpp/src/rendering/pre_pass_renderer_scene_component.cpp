#include <babylon/rendering/pre_pass_renderer_scene_component.h>

#include <babylon/engines/scene.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

PrePassRendererSceneComponent::PrePassRendererSceneComponent(Scene* iScene) : scene{iScene}
{
}

PrePassRendererSceneComponent::~PrePassRendererSceneComponent() = default;

void PrePassRendererSceneComponent::_register()
{
  scene->_beforeCameraDrawStage.registerStep(SceneComponentConstants::STEP_BEFORECAMERADRAW_PREPASS,
                                             this, [this](Camera * /*camera*/) -> bool {
                                               _beforeCameraDraw();
                                               return true;
                                             });
  scene->_afterCameraDrawStage.registerStep(SceneComponentConstants::STEP_AFTERCAMERADRAW_PREPASS,
                                            this, [this](Camera * /*camera*/) -> bool {
                                              _afterCameraDraw();
                                              return true;
                                            });
  scene->_beforeClearStage.registerStep(SceneComponentConstants::STEP_BEFORECLEARSTAGE_PREPASS,
                                        this, [this]() -> void { _beforeClearStage(); });
}

void PrePassRendererSceneComponent::_beforeCameraDraw()
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_beforeCameraDraw();
  }
}

void PrePassRendererSceneComponent::_afterCameraDraw()
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_afterCameraDraw();
  }
}

void PrePassRendererSceneComponent::_beforeClearStage()
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->clear();
  }
}

void PrePassRendererSceneComponent::serialize(json& /*serializationObject*/)
{
}

void PrePassRendererSceneComponent::addFromContainer(AbstractScene& /*container*/)
{
  // Nothing to do
}

void PrePassRendererSceneComponent::removeFromContainer(AbstractScene& /*container*/,
                                                        bool /*dispose*/)
{
  // Make sure nothing will be serialized
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->subSurfaceConfiguration->clearAllDiffusionProfiles();
  }
}

void PrePassRendererSceneComponent::rebuild()
{
  // Nothing to do for this component
}

void PrePassRendererSceneComponent::dispose()
{
  // Nothing to do for this component
}

} // end of namespace BABYLON
