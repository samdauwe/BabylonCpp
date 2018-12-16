#ifndef BABYLON_SAMPLES_TEXTURES_ENVIRONMENT_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_ENVIRONMENT_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Environment texture scene. Example demonstrating how
 * to create a skybox using and environment texture.
 * @see https://www.babylonjs-playground.com/#09R6ZA#3
 */
struct EnvironmentTextureScene : public IRenderableScene {

  EnvironmentTextureScene(ICanvas* iCanvas);
  ~EnvironmentTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct EnvironmentTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_ENVIRONMENT_TEXTURE_SCENE_H
