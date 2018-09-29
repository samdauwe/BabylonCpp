#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PerlinNoiseProceduralTextureScene Scene. Example demonstrating how to
 * use perlin noise procedural texture.
 */
struct PerlinNoiseProceduralTextureScene : public IRenderableScene {

  PerlinNoiseProceduralTextureScene(ICanvas* iCanvas);
  ~PerlinNoiseProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PerlinNoiseProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PROCEDURAL_TEXTURE_SCENE_H
