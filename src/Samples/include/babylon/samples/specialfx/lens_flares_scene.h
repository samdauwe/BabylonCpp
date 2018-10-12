#ifndef BABYLON_SAMPLES_SPECIAL_FX_LENS_FLARES_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_LENS_FLARES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Lens Flares Scene. This example demonstrates how to simulate lens
 * flares on the camera.
 * @see https://www.babylonjs-playground.com/#ZEB7H6#0
 */
struct LensFlaresScene : public IRenderableScene {

  LensFlaresScene(ICanvas* iCanvas);
  ~LensFlaresScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct LensFlaresScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_LENS_FLARES_SCENE_H
