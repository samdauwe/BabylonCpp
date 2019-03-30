#ifndef BABYLON_SAMPLES_MATERIALS_PBR_SPECULAR_GLOSSINESS_TEXTURE_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_PBR_SPECULAR_GLOSSINESS_TEXTURE_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR Specular Glossiness Texture Material Scene. Example demonstrating
 * how to create physically based rendering specular glossiness materials.
 * @see https://www.babylonjs-playground.com/#Z1VL3V#4
 * @see https://doc.babylonjs.com/how_to/physically_based_rendering
 */
struct PBRSpecularGlossinessTextureMaterialScene : public IRenderableScene {

  PBRSpecularGlossinessTextureMaterialScene(ICanvas* iCanvas);
  ~PBRSpecularGlossinessTextureMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PBRSpecularGlossinessTextureMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_PBR_SPECULAR_GLOSSINESS_TEXTURE_MATERIAL_SCENE_H
