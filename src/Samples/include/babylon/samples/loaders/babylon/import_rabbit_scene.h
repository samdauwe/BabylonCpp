#ifndef BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_RABBIT_SCENE_H
#define BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_RABBIT_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Rabbit Scene.
 * @see https://www.babylonjs-playground.com/#NMU4ZM#1
 */
struct ImportRabbitScene : public IRenderableScene {

  ImportRabbitScene(ICanvas* iCanvas);
  ~ImportRabbitScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportRabbitScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_RABBIT_SCENE_H
