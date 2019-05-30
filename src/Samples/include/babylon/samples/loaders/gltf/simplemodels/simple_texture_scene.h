#ifndef BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Texture Scene (glTF).
 * Note that the texture image also shows the texture coordinates:
 * * The upper left point of the texture image has the texture coordinates (0,0)
 * * The lower right point of the texture image has the texture coordinates
 *   (1,1)
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/cx20/gltf-test/tree/master/tutorialModels/SimpleTexture
 */
struct SimpleTextureScene : public IRenderableScene {

  SimpleTextureScene(ICanvas* iCanvas);
  ~SimpleTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SimpleTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_TEXTURE_SCENE_H
