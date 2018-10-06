#ifndef BABYLON_SAMPLES_SPECIAL_FX_ENVIRONMENT_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_ENVIRONMENT_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Environment scene. This example demonstrates how to add a skybox and
 * fog.
 * @see https://www.babylonjs-playground.com/#7G0IQW#0
 */
class EnvironmentScene : public IRenderableScene {

public:
  EnvironmentScene(ICanvas* iCanvas);
  ~EnvironmentScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _alpha;

}; // end of class EnvironmentScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_ENVIRONMENT_SCENE_H
