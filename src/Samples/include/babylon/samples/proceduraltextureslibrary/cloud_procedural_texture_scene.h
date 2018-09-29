#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief CloudProceduralTextureScene Scene. Example demonstrating how to use
 * cloud procedural texture.
 * @see https://www.babylonjs-playground.com/##NQDNM#0
 */
struct CloudProceduralTextureScene : public IRenderableScene {

  CloudProceduralTextureScene(ICanvas* iCanvas);
  ~CloudProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CloudProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_PROCEDURAL_TEXTURE_SCENE_H
