#ifndef BABYLON_SAMPLES_MATERIALS_PBR_METALLIC_ROUGHNESS_GOLD_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_PBR_METALLIC_ROUGHNESS_GOLD_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR Metallic Roughness Gold Material Scene. Example demonstrating how
 * to create physically based rendering metallic roughness materials and add
 * more roughness to give it a more golden look.
 * @see https://www.babylonjs-playground.com/#2FDQT5#12
 * @see https://doc.babylonjs.com/how_to/physically_based_rendering
 */
struct PBRMetallicRoughnessGoldMaterialScene : public IRenderableScene {

  PBRMetallicRoughnessGoldMaterialScene(ICanvas* iCanvas);
  ~PBRMetallicRoughnessGoldMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PBRMetallicRoughnessGoldMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_PBR_METALLIC_ROUGHNESS_GOLD_MATERIAL_SCENE_H
