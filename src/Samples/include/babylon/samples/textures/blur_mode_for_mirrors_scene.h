#ifndef BABYLON_SAMPLES_TEXTURES_BLUR_MODE_FOR_MIRRORS_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_BLUR_MODE_FOR_MIRRORS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Blur Mode For Mirrors Scene. Example demonstrating how to use
 * blur mode for mirrors.
 * @see https://www.babylonjs-playground.com/#9I6NX1
 */
class BlurModeForMirrorsScene : public IRenderableScene {

public:
  BlurModeForMirrorsScene(ICanvas* iCanvas);
  ~BlurModeForMirrorsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class BlurModeForMirrorsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_BLUR_MODE_FOR_MIRRORS_SCENE_H
