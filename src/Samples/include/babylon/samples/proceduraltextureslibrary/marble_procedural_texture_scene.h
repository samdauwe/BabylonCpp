#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief MarbleProceduralTextureScene Scene. Example demonstrating how to use
 * marble procedural texture.
 * @see https://www.babylonjs-playground.com/#HS1SK#4
 */
struct MarbleProceduralTextureScene : public IRenderableScene {

  MarbleProceduralTextureScene(ICanvas* iCanvas);
  ~MarbleProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct MarbleProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_PROCEDURAL_TEXTURE_SCENE_H
