#ifndef BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_CANDLE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_CANDLE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Candle Scene.
 * @see https://www.babylonjs-playground.com/#TSCJPP#1
 */
struct ImportCandleScene : public IRenderableScene {

  ImportCandleScene(ICanvas* iCanvas);
  ~ImportCandleScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportCandleScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_CANDLE_SCENE_H
