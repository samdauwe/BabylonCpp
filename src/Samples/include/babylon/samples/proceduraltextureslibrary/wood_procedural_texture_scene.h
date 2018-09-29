#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_WOOD_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_WOOD_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief WoodProceduralTextureScene Scene. Example demonstrating how to use
 * wood procedural texture.
 * @see https://www.babylonjs-playground.com/#K41IJ
 */
struct WoodProceduralTextureScene : public IRenderableScene {

  WoodProceduralTextureScene(ICanvas* iCanvas);
  ~WoodProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct WoodProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_WOOD_PROCEDURAL_TEXTURE_SCENE_H
