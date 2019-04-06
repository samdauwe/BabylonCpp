#ifndef BABYLON_SAMPLES_ANIMATIONS_CARTOON_ANIMATIONS_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_CARTOON_ANIMATIONS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cartoon Animations Scene. Example demonstrating how to combining a
 * number of clips to form a cartoon is to give start times to each animation
 * clip
 * @see https://www.babylonjs-playground.com/#2L26P1#8
 * @see https://doc.babylonjs.com/how_to/sequence#cartoon
 */
struct CartoonAnimationsScene : public IRenderableScene {

public:
  CartoonAnimationsScene(ICanvas* iCanvas);
  ~CartoonAnimationsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CartoonAnimationsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_CARTOON_ANIMATIONS_SCENE_H
