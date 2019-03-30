#ifndef BABYLON_SAMPLES_MATERIALS_PBR_METALLIC_ROUGHNESS_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_PBR_METALLIC_ROUGHNESS_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR Metallic Roughness Material. Example demonstrating how to create
 * physically based rendering metallic roughness materials.
 * @see https://www.babylonjs-playground.com/#2FDQT5#0
 * @see https://doc.babylonjs.com/how_to/physically_based_rendering
 */
struct PBRMetallicRoughnessMaterialScene : public IRenderableScene {

  PBRMetallicRoughnessMaterialScene(ICanvas* iCanvas);
  ~PBRMetallicRoughnessMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PBRMetallicRoughnessMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_PBR_METALLIC_ROUGHNESS_MATERIAL_SCENE_H
