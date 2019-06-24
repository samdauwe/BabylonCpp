#ifndef BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_ANIMATED_CUBE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_ANIMATED_CUBE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Animated Cube Scene (glTF). A cube with non-smoothed faces, having a
 * linear rotation animation.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube
 */
struct AnimatedCubeScene : public IRenderableScene {

  AnimatedCubeScene(ICanvas* iCanvas);
  ~AnimatedCubeScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct AnimatedCubeScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_ANIMATED_CUBE_SCENE_H
