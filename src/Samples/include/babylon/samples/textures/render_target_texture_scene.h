#ifndef BABYLON_SAMPLES_TEXTURES_RENDER_TARGET_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_RENDER_TARGET_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Render Target Texture Scene. Example demonstrating how to render to a
 * texture.
 * @see https://www.babylonjs-playground.com/#1ELSIY#0
 */
struct RenderTargetTextureScene : public IRenderableScene {

  RenderTargetTextureScene(ICanvas* iCanvas);
  ~RenderTargetTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct RenderTargetTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_RENDER_TARGET_TEXTURE_SCENE_H
