#ifndef BABYLON_SAMPLES_TEXTURES_TWO_SIDED_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_TWO_SIDED_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Two Sided Texture Scene. Example demonstrating how to have different
 * textures on the front and back of a mesh.
 * @see https://www.babylonjs-playground.com/#LXZPJK#3
 * @see https://doc.babylonjs.com/how_to/frontandbackuv
 */
struct TwoSidedTextureScene : public IRenderableScene {

  TwoSidedTextureScene(ICanvas* iCanvas);
  ~TwoSidedTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct TwoSidedTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_TWO_SIDED_TEXTURE_SCENE_H
