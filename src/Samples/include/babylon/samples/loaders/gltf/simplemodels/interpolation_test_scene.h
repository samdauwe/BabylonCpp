#ifndef BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_INTERPOLATION_TEST_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_INTERPOLATION_TEST_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Interpolation Test Scene (glTF). A sample with three different
 * animation interpolations.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/InterpolationTest
 */
struct InterpolationTestScene : public IRenderableScene {

  InterpolationTestScene(ICanvas* iCanvas);
  ~InterpolationTestScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct InterpolationTestScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_INTERPOLATION_TEST_SCENE_H
