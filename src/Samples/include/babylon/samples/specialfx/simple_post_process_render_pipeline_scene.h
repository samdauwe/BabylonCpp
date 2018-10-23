#ifndef BABYLON_SAMPLES_SPECIAL_FX_SIMPLE_POST_PROCESS_RENDER_PIPELINE_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_SIMPLE_POST_PROCESS_RENDER_PIPELINE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class PostProcess;
using PostProcessPtr = std::shared_ptr<PostProcess>;

namespace Samples {

/**
 * @brief Simple post process render pipeline scene. This example demonstrates
 * how to use the post process renders pipeline.
 * @see https://www.babylonjs-playground.com/#QCGFI6
 */
class SimplePostProcessRenderPipelineScene : public IRenderableScene {

public:
  SimplePostProcessRenderPipelineScene(ICanvas* iCanvas);
  ~SimplePostProcessRenderPipelineScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  PostProcessPtr _blackAndWhite, _horizontalBlur;

}; // end of class SimplePostProcessRenderPipelineScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_SPECIAL_FX_SIMPLE_POST_PROCESS_RENDER_PIPELINE_SCENE_H
