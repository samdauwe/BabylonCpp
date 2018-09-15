#ifndef BABYLON_SAMPLES_MESHES_CIRCLE_CURVES_FROM_BEZIERS_SCENE_H
#define BABYLON_SAMPLES_MESHES_CIRCLE_CURVES_FROM_BEZIERS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class LinesMesh;
using LinesMeshPtr = std::shared_ptr<LinesMesh>;

namespace Samples {

class CircleCurvesFromBeziersScene : public IRenderableScene {

public:
  CircleCurvesFromBeziersScene(ICanvas* iCanvas);
  ~CircleCurvesFromBeziersScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _speed;
  LinesMeshPtr _circle1, _circle2;

}; // end of class CircleCurvesFromBeziersScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_CIRCLE_CURVES_FROM_BEZIERS_SCENE_H
