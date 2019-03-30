#ifndef BABYLON_SAMPLES_SPECIAL_FX_WAVE_OF_SPRITES_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_WAVE_OF_SPRITES_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class EventState;
class PointerInfo;

namespace Samples {

/**
 * @brief Wave Of Sprites Scene. Example demonstrating how to Load and display
 * sprites.
 * @see https://www.babylonjs-playground.com/#C8ZSHY#1
 * @see https://doc.babylonjs.com/babylon101/sprites
 */
struct WaveOfSpritesScene : public IRenderableScene {

  WaveOfSpritesScene(ICanvas* iCanvas);
  ~WaveOfSpritesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct WaveOfSpritesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_WAVE_OF_SPRITES_SCENE_H
