#ifndef BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_DUMMY3_SCENE_H
#define BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_DUMMY3_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Dummy 3 Scene.
 * @see https://www.babylonjs-playground.com/#C38BUD#1
 */
struct ImportDummy3Scene : public IRenderableScene {

  ImportDummy3Scene(ICanvas* iCanvas);
  ~ImportDummy3Scene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportDummy3Scene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_DUMMY3_SCENE_H
