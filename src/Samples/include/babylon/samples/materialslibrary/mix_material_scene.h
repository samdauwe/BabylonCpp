#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_MIX_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_MIX_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the mix material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#1DFTDT
 */
struct MixMaterialScene : public IRenderableScene {

  MixMaterialScene(ICanvas* iCanvas);
  ~MixMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct MixMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_MIX_MATERIAL_SCENE_H
