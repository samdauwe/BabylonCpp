#ifndef BABYLON_SAMPLES_SPECIAL_FX_REFRACTION_AND_REFLECTION_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_REFRACTION_AND_REFLECTION_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Refraction and reflection scene. This example simulates how light
 * would reflect and refract with a sphere.
 * @see https://www.babylonjs-playground.com/#XH85A9#0
 */
class RefractionAndReflectionScene : public IRenderableScene {

public:
  RefractionAndReflectionScene(ICanvas* iCanvas);
  ~RefractionAndReflectionScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class RefractionAndReflectionScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_REFRACTION_AND_REFLECTION_SCENE_H
