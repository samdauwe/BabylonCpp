#ifndef BABYLON_SAMPLES_LIGHTS_HEMISPHERIC_LIGHT_SCENE_H
#define BABYLON_SAMPLES_LIGHTS_HEMISPHERIC_LIGHT_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the hemispheric light. This light
 * simulate an ambient environment light.
 * @see https://www.babylonjs-playground.com/#20OAV9#5
 * @see https://doc.babylonjs.com/babylon101/lights#the-hemispheric-light
 */
struct HemisphericLightScene : public IRenderableScene {

  HemisphericLightScene(ICanvas* iCanvas);
  ~HemisphericLightScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct HemisphericLightScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LIGHTS_HEMISPHERIC_LIGHT_SCENE_H
