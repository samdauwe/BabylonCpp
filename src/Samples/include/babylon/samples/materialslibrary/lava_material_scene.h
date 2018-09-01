#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_LAVA_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_LAVA_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the lava material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#1BLVWO#25
 */
struct LavaMaterialScene : public IRenderableScene {

  LavaMaterialScene(ICanvas* iCanvas);
  ~LavaMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct LavaMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_LAVA_MATERIAL_SCENE_H
