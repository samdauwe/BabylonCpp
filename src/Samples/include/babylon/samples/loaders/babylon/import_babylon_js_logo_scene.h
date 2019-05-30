#ifndef BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_BABYLON_JS_LOGO_SCENE_H
#define BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_BABYLON_JS_LOGO_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import BabylonJS Logo Scene.
 * @see https://www.babylonjs-playground.com/#17LHMG#1
 */
struct ImportBabylonJSLogoScene : public IRenderableScene {

  ImportBabylonJSLogoScene(ICanvas* iCanvas);
  ~ImportBabylonJSLogoScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportBabylonJSLogoScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_BABYLON_JS_LOGO_SCENE_H
