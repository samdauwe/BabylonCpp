#ifndef BABYLON_SAMPLES_CAMERAS_MULTI_VIEWS_SCENE_H
#define BABYLON_SAMPLES_CAMERAS_MULTI_VIEWS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Multi-Views Scene. Example demonstrating how to how to use multi
 * views.
 * @see https://www.babylonjs-playground.com/#E9IRIF
 */
struct MultiViewsScene : public IRenderableScene {

  MultiViewsScene(ICanvas* iCanvas);
  ~MultiViewsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct MultiViewsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CAMERAS_MULTI_VIEWS_SCENE_H
