#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_BRICK_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_BRICK_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief BrickProceduralTextureScene Scene. Example demonstrating how to use
 * Brick procedural texture.
 * @see https://www.babylonjs-playground.com/#1CL0BO
 */
struct BrickProceduralTextureScene : public IRenderableScene {

  BrickProceduralTextureScene(ICanvas* iCanvas);
  ~BrickProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BrickProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_BRICK_PROCEDURAL_TEXTURE_SCENE_H
