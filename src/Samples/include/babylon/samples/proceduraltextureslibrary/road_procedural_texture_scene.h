#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_ROAD_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_ROAD_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief RoadProceduralTextureScene Scene. Example demonstrating how to use
 * road procedural texture.
 * @see https://www.babylonjs-playground.com/##FBW4N#0
 */
struct RoadProceduralTextureScene : public IRenderableScene {

  RoadProceduralTextureScene(ICanvas* iCanvas);
  ~RoadProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct RoadProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_ROAD_PROCEDURAL_TEXTURE_SCENE_H
