#ifndef BABYLON_POSTPROCESSES_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_SCENE_COMPONENT_H
#define BABYLON_POSTPROCESSES_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_SCENE_COMPONENT_H

#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

FWD_CLASS_SPTR(PostProcessRenderPipelineManagerSceneComponent)
FWD_CLASS_SPTR(RenderTargetTexture)

/**
 * @brief Defines the Render Pipeline scene component responsible to rendering pipelines.
 */
class BABYLON_SHARED_EXPORT PostProcessRenderPipelineManagerSceneComponent
    : public ISceneComponent {

public:
  /**
   * The component name helpful to identify the component in the list of scene components.
   */
  static constexpr const char* name
    = SceneComponentConstants::NAME_POSTPROCESSRENDERPIPELINEMANAGER;

public:
  template <typename... Ts>
  static PostProcessRenderPipelineManagerSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<PostProcessRenderPipelineManagerSceneComponent>(
      new PostProcessRenderPipelineManagerSceneComponent(std::forward<Ts>(args)...));
  }
  ~PostProcessRenderPipelineManagerSceneComponent() override; // = default

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Creates a new instance of the component for the given scene
   * @param scene Defines the scene to register the component in
   */
  PostProcessRenderPipelineManagerSceneComponent(Scene* scene);

private:
  void _gatherRenderTargets();

}; // end of class PostProcessRenderPipelineManagerSceneComponent

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESSES_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_SCENE_COMPONENT_H
