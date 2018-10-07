#ifndef BABYLON_SAMPLES_SPECIAL_FX_FRESNEL_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_FRESNEL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Fresnel scene. This example demonstrates how to render spheres to
 * simulate a fresnel lens.
 * @see https://www.babylonjs-playground.com/#AQZJ4C#0
 */
struct FresnelScene : public IRenderableScene {

  FresnelScene(ICanvas* iCanvas);
  ~FresnelScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct FresnelScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_FRESNEL_SCENE_H
