#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager_scene_component.h>

#include <babylon/engines/scene.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager.h>

namespace BABYLON {

PostProcessRenderPipelineManagerSceneComponent::
  PostProcessRenderPipelineManagerSceneComponent(Scene* iScene)
{
  ISceneComponent::name = PostProcessRenderPipelineManagerSceneComponent::name;
  scene                 = iScene;
}

PostProcessRenderPipelineManagerSceneComponent::
PostProcessRenderPipelineManagerSceneComponent::~PostProcessRenderPipelineManagerSceneComponent() = default;

void PostProcessRenderPipelineManagerSceneComponent::_register()
{
  scene->_gatherRenderTargetsStage.registerStep(
    SceneComponentConstants::
      STEP_GATHERRENDERTARGETS_POSTPROCESSRENDERPIPELINEMANAGER,
    this, [this](const std::vector<RenderTargetTexturePtr>& /*renderTargets*/) {
      _gatherRenderTargets();
    });
}

void PostProcessRenderPipelineManagerSceneComponent::rebuild()
{
  if (scene->postProcessRenderPipelineManager()) {
    scene->postProcessRenderPipelineManager()->_rebuild();
  }
}

void PostProcessRenderPipelineManagerSceneComponent::dispose()
{
  if (scene->postProcessRenderPipelineManager()) {
    scene->postProcessRenderPipelineManager()->dispose();
  }
}

void PostProcessRenderPipelineManagerSceneComponent::_gatherRenderTargets()
{
  if (scene->postProcessRenderPipelineManager()) {
    scene->postProcessRenderPipelineManager()->update();
  }
}

} // end of namespace BABYLON
