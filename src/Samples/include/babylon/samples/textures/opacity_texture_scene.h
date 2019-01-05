#ifndef BABYLON_SAMPLES_TEXTURES_OPACITY_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_OPACITY_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Opacity Textur Scene. Example demonstrating how an image with a
 * transparency gradient can be applied to a material using opacityTexture.
 * @see http://www.babylonjs-playground.com/#20OAV9#24
 */
struct OpacityTexturScene : public IRenderableScene {

  OpacityTexturScene(ICanvas* iCanvas);
  ~OpacityTexturScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct OpacityTexturScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_OPACITY_TEXTURE_SCENE_H
