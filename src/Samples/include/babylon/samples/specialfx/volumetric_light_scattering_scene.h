#ifndef BABYLON_SAMPLES_SPECIAL_FX_VOLUMETRIC_LIGHT_SCATTERING_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_VOLUMETRIC_LIGHT_SCATTERING_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;

namespace Samples {

/**
 * @brief Volumetric Light Scattering Scene. This example demonstrates how to
 * simulate light scattering due to light hitting the atmosphere.
 * @see https://www.babylonjs-playground.com/#V2DAKC#0
 */
class VolumetricLightScatteringScene : public IRenderableScene {

public:
  VolumetricLightScatteringScene(ICanvas* iCanvas);
  ~VolumetricLightScatteringScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;

}; // end of class VolumetricLightScatteringScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_VOLUMETRIC_LIGHT_SCATTERING_SCENE_H
