#ifndef BABYLON_SAMPLES_MATERIALS_X_RAY_MATERIAL_WITH_FRESNEL_SKULL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_X_RAY_MATERIAL_WITH_FRESNEL_SKULL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of fresnel parameters to simulate an x-ray
 * effect.
 * @see https://www.babylonjs-playground.com/#GK7FK#0
 */
class XRayMaterialWithFresnelSkullScene : public IRenderableScene {

public:
  XRayMaterialWithFresnelSkullScene(ICanvas* iCanvas);
  ~XRayMaterialWithFresnelSkullScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  FreeCamera* _camera;
  StandardMaterial* _xrayMat;

}; // end of class XRayMaterialWithFresnelSkullScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_X_RAY_MATERIAL_WITH_FRESNEL_SKULL_SCENE_H
