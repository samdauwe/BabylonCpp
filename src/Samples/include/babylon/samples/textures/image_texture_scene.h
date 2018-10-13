#ifndef BABYLON_SAMPLES_TEXTURES_IMAGE_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_IMAGE_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Image Texture Scene. Example demonstrating how to apply an image with
 * transparency to a mesh.
 * @see https://www.babylonjs-playground.com/#YDO1F#75
 */
struct ImageTextureScene : public IRenderableScene {

  ImageTextureScene(ICanvas* iCanvas);
  ~ImageTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImageTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_IMAGE_TEXTURE_SCENE_H
