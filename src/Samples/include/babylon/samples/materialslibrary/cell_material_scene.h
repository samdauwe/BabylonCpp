#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_CELL_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_CELL_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the cell material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#36VUUE#0
 */
struct CellMaterialScene : public IRenderableScene {

  CellMaterialScene(ICanvas* iCanvas);
  ~CellMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CellMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_CELL_MATERIAL_SCENE_H
