#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_GRASS_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_GRASS_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief GrassProceduralTextureScene Scene. Example demonstrating how to use
 * grass procedural texture.
 * @see https://www.babylonjs-playground.com/#KM3TC#1
 */
struct GrassProceduralTextureScene : public IRenderableScene {

  GrassProceduralTextureScene(ICanvas* iCanvas);
  ~GrassProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct GrassProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_GRASS_PROCEDURAL_TEXTURE_SCENE_H
