#ifndef BABYLON_SAMPLES_CAMERAS_CAMERAS_SCENE_H
#define BABYLON_SAMPLES_CAMERAS_CAMERAS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cameras scene demonstrating the types of cameras in babylonjs.
 * @see https://www.babylonjs-playground.com/#1A3M5C#0
 * @see https://doc.babylonjs.com/babylon101/cameras
 */
struct CamerasScene : public IRenderableScene {

  CamerasScene(ICanvas* iCanvas);
  ~CamerasScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CamerasScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CAMERAS_CAMERAS_SCENE_H
