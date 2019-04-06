#ifndef BABYLON_SAMPLES_ANIMATIONS_EASING_SHAPE_EASINGS_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_EASING_SHAPE_EASINGS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;

namespace Samples {

/**
 * @brief Shape Easings Scene. Example demonstarting how to create animations
 * with different easing functions.
 * @see https://www.babylonjs-playground.com/#HH1U5#62
 * @see https://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class ShapeEasingsScene : public IRenderableScene {

public:
  ShapeEasingsScene(ICanvas* iCanvas);
  ~ShapeEasingsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;

}; // end of class ShapeEasingsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_EASING_SHAPE_EASINGS_SCENE_H
