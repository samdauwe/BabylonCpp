#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRID_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRID_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the grid material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#1UFGZH#12
 */
class GridMaterialScene : public IRenderableScene {

public:
  GridMaterialScene(ICanvas* iCanvas);
  ~GridMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;

}; // end of class GridMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRID_MATERIAL_SCENE_H
