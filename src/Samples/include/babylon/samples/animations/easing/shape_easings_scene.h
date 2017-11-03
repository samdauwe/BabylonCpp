#ifndef BABYLON_SAMPLES_ANIMATIONS_EASING_SHAPE_EASINGS_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_EASING_SHAPE_EASINGS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class ShapeEasingsScene : public IRenderableScene {

public:
  ShapeEasingsScene(ICanvas* iCanvas);
  ~ShapeEasingsScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCamera* _camera;

}; // end of class ShapeEasingsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_EASING_SHAPE_EASINGS_SCENE_H
