#ifndef BABYLON_SAMPLES_SPECIAL_FX_KERNEL_BASED_BLUR_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_KERNEL_BASED_BLUR_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class PointLight;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using PointLightPtr      = std::shared_ptr<PointLight>;

namespace Samples {

/**
 * @brief Kernel Based Blur Scene. Example demonstrating how to achieve kernel
 * based blur using blur postprocess.
 * @see https://www.babylonjs-playground.com/#FBH4J7#1
 */
class KernelBasedBlurScene : public IRenderableScene {

public:
  KernelBasedBlurScene(ICanvas* iCanvas);
  ~KernelBasedBlurScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  PointLightPtr _light;
  ArcRotateCameraPtr _camera;

}; // end of class KernelBasedBlurScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_KERNEL_BASED_BLUR_SCENE_H
