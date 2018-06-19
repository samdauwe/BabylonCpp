#ifndef BABYLON_SAMPLES_MESHES_ROTATION_AND_SCALING_SCENE_H
#define BABYLON_SAMPLES_MESHES_ROTATION_AND_SCALING_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Rotation and scaling scene. This example demonstrates how to position
 * mesh layout in space.
 * @see https://www.babylonjs-playground.com/#CURCZC#0
 */
struct RotationAndScalingScene : public IRenderableScene {

  RotationAndScalingScene(ICanvas* iCanvas);
  ~RotationAndScalingScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct RotationAndScalingScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_ROTATION_AND_SCALING_SCENE_H
