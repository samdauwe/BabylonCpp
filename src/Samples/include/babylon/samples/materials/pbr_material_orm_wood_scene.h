#ifndef BABYLON_SAMPLES_MATERIALS_PBR_MATERIAL_ORM_WOOD_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_PBR_MATERIAL_ORM_WOOD_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBRM material ORM Wood Scene. Example demonstrating how to manually
 * create an ORM texture linked to the metallicTexture channel:
 *   - Red channel for Ambient Occlusion
 *   - Green channel for Roughness
 *   - Blue channel for Metallic
 * @see https://www.babylonjs-playground.com/#K4S3GU#58
 * @see
 * https://forum.babylonjs.com/t/how-to-create-a-pbr-from-multiple-images/1433
 */
struct PBRMaterialORMWoodScene : public IRenderableScene {

  PBRMaterialORMWoodScene(ICanvas* iCanvas);
  ~PBRMaterialORMWoodScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PBRMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_PBR_MATERIAL_ORM_WOOD_SCENE_H
