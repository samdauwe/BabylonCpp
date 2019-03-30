#ifndef BABYLON_SAMPLES_MATERIALS_PBR_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_PBR_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR material scene. Example demonstrating how to create physically
 * based rendering materials.
 * @see https://www.babylonjs-playground.com/#8MGKWK#0
 * @see https://doc.babylonjs.com/how_to/physically_based_rendering
 */
struct PBRMaterialScene : public IRenderableScene {

  PBRMaterialScene(ICanvas* iCanvas);
  ~PBRMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PBRMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_PBR_MATERIAL_SCENE_H
