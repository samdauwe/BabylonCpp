#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_CELL_SHADING_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_CELL_SHADING_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cell shading material scene. Scene demonstrating the use of the cell
 * material from the materials library.
 * @see https://www.babylonjs-playground.com/#0ZB1A3#1
 * @see https://doc.babylonjs.com/extensions/cell
 */
struct CellShadingMaterialScene : public IRenderableScene {

  CellShadingMaterialScene(ICanvas* iCanvas);
  ~CellShadingMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CellShadingMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_CELL_SHADING_MATERIAL_SCENE_H
