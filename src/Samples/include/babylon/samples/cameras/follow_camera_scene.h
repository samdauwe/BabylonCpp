#ifndef BABYLON_SAMPLES_CAMERAS_FOLLOW_CAMERA_SCENE_H
#define BABYLON_SAMPLES_CAMERAS_FOLLOW_CAMERA_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
class FollowCamera;
using MeshPtr         = std::shared_ptr<Mesh>;
using FollowCameraPtr = std::shared_ptr<FollowCamera>;

namespace Samples {

/**
 * @brief Follow camera scene. Cameras scene demonstrating a follow camera
 * following a moving target.
 * @see https://www.babylonjs-playground.com/#12WBC#84
 */
class FollowCameraScene : public IRenderableScene {

public:
  FollowCameraScene(ICanvas* iCanvas);
  ~FollowCameraScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _box;
  FollowCameraPtr _camera;
  // Box movement variables
  float _alpha;
  float _orbitRadius;

}; // end of class FollowCameraScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CAMERAS_FOLLOW_CAMERA_SCENE_H
