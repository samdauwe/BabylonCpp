#ifndef BABYLON_SAMPLES_SPECIAL_FX_SPRITES_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_SPRITES_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class EventState;
class PointerInfo;

namespace Samples {

/**
 * @brief Sprites scene. Example demonstrating how to Load and display sprites.
 * @see https://www.babylonjs-playground.com/#9RI8CG#0
 * @see https://doc.babylonjs.com/babylon101/sprites
 */
class SpritesScene : public IRenderableScene {

public:
  SpritesScene(ICanvas* iCanvas);
  ~SpritesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  std::function<void(PointerInfo* p, EventState& es)> _pointerInput;

}; // end of class SpritesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_SPRITES_SCENE_H
