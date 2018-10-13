#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_WATER_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_WATER_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the water material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#L76FB1#0
 */
struct WaterMaterialScene : public IRenderableScene {

  WaterMaterialScene(ICanvas* iCanvas);
  ~WaterMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct WaterMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_WATER_MATERIAL_SCENE_H
