#ifndef BABYLON_SAMPLES_LIGHTS_DIRECTIONAL_LIGHT_SCENE_H
#define BABYLON_SAMPLES_LIGHTS_DIRECTIONAL_LIGHT_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the directional light. The light is
 * emitted from everywhere in the specified direction, and has an infinite
 * range.
 * @see https://www.babylonjs-playground.com/#20OAV9#1
 */
struct DirectionalLightScene : public IRenderableScene {

  DirectionalLightScene(ICanvas* iCanvas);
  ~DirectionalLightScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct DirectionalLightScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LIGHTS_DIRECTIONAL_LIGHT_SCENE_H
