#ifndef BABYLON_SAMPLES_OPTIMIZATIONS_LEVEL_OF_DETAIL_SCENE_H
#define BABYLON_SAMPLES_OPTIMIZATIONS_LEVEL_OF_DETAIL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Level of detail Scene. Example demonstrating how to use various meshes
 * based on distance to optimize rendering speed.
 * @see https://www.babylonjs-playground.com/#7HMHAU#1
 * @see https://doc.babylonjs.com/how_to/how_to_use_lod
 */
struct LevelOfDetailScene : public IRenderableScene {

  LevelOfDetailScene(ICanvas* iCanvas);
  ~LevelOfDetailScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct LevelOfDetailScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_OPTIMIZATIONS_LEVEL_OF_DETAIL_SCENE_H
