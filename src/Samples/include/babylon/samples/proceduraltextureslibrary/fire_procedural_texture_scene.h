#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_FIRE_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_FIRE_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief FireProceduralTextureScene Scene. Example demonstrating how to use
 * fire procedural texture.
 * @see https://www.babylonjs-playground.com/#KM3TC
 */
struct FireProceduralTextureScene : public IRenderableScene {

  FireProceduralTextureScene(ICanvas* iCanvas);
  ~FireProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct FireProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_FIRE_PROCEDURAL_TEXTURE_SCENE_H
