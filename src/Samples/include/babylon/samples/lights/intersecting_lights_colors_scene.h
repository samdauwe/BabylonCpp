#ifndef BABYLON_SAMPLES_LIGHTS_INTERSECTING_LIGHTS_COLORS_SCENE_H
#define BABYLON_SAMPLES_LIGHTS_INTERSECTING_LIGHTS_COLORS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Intersecting Lights Colors Scene. Scene demonstrating intersecting
 * lights colors using intersecting spot lights.
 * @see http://www.babylonjs-playground.com/#20OAV9#9
 */
struct IntersectingLightsColorsScene : public IRenderableScene {

  IntersectingLightsColorsScene(ICanvas* iCanvas);
  ~IntersectingLightsColorsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct IntersectingLightsColorsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LIGHTS_INTERSECTING_LIGHTS_COLORS_SCENE_H
