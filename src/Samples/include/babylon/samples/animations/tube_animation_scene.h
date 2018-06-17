#ifndef BABYLON_SAMPLES_ANIMATIONS_TUBE_ANIMATION_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_TUBE_ANIMATION_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct TubeAnimationScene : public IRenderableScene {

  TubeAnimationScene(ICanvas* iCanvas);
  ~TubeAnimationScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct TubeAnimationScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_TUBE_ANIMATION_SCENE_H
