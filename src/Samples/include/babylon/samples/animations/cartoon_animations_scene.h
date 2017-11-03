#ifndef BABYLON_SAMPLES_ANIMATIONS_CARTOON_ANIMATIONS_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_CARTOON_ANIMATIONS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct CartoonAnimationsScene : public IRenderableScene {

public:
  CartoonAnimationsScene(ICanvas* iCanvas);
  ~CartoonAnimationsScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CartoonAnimationsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_CARTOON_ANIMATIONS_SCENE_H
