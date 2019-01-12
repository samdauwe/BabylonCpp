#ifndef BABYLON_SAMPLES_LOADERS_IMPORT_DUDE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_IMPORT_DUDE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Dude Scene.
 * @see https://www.babylonjs-playground.com/#WLDCUC#2
 */
struct ImportDudeScene : public IRenderableScene {

  ImportDudeScene(ICanvas* iCanvas);
  ~ImportDudeScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportDudeScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_IMPORT_DUDE_SCENE_H
