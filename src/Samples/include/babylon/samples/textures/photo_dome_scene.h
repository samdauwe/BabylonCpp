#ifndef BABYLON_SAMPLES_TEXTURES_PHOTO_DOME_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_PHOTO_DOME_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Photo Dome Scene. Example demonstrating how to use easily display and
 * control 360 photos.
 * @see https://www.babylonjs-playground.com/#14KRGG#3
 */
class PhotoDomeScene : public IRenderableScene {

public:
  PhotoDomeScene(ICanvas* iCanvas);
  ~PhotoDomeScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class PhotoDomeScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_PHOTO_DOME_SCENE_H
