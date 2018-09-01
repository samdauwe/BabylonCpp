#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_TERRAIN_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_TERRAIN_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the terrain material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#E6OZX#7
 */
struct TerrainMaterialScene : public IRenderableScene {

  TerrainMaterialScene(ICanvas* iCanvas);
  ~TerrainMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct TerrainMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_TERRAIN_MATERIAL_SCENE_H
