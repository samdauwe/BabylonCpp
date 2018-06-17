#ifndef BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct AnimationsScene : public IRenderableScene {

public:
  AnimationsScene(ICanvas* iCanvas);
  ~AnimationsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct AnimationsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_SCENE_H
