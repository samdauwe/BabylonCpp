#ifndef BABYLON_SAMPLES_LIGHTS_POINT_LIGHT_SCENE_H
#define BABYLON_SAMPLES_LIGHTS_POINT_LIGHT_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the point light. This light is emitting
 * from a single point.
 * @see https://www.babylonjs-playground.com/#20OAV9#0
 */
struct PointLightScene : public IRenderableScene {

  PointLightScene(ICanvas* iCanvas);
  ~PointLightScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PointLightScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LIGHTS_POINT_LIGHT_SCENE_H
