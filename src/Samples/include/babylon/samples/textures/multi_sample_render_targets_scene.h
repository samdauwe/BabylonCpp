#ifndef BABYLON_SAMPLES_TEXTURES_MULTI_SAMPLE_RENDER_TARGETS_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_MULTI_SAMPLE_RENDER_TARGETS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Multisample Render Targets Scene. Example demonstrating how to use
 * multisample render targets.
 * @see https://www.babylonjs-playground.com/#12MKMN
 */
class MultiSampleRenderTargetsScene : public IRenderableScene {

public:
  MultiSampleRenderTargetsScene(ICanvas* iCanvas);
  ~MultiSampleRenderTargetsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class MultiSampleRenderTargetsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_MULTI_SAMPLE_RENDER_TARGETS_SCENE_H
