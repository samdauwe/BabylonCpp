#ifndef BABYLON_SAMPLES_LOADERS_FEATURE_TEST_MODELS_ALPHA_BLEND_MODE_TEST_SCENE_H
#define BABYLON_SAMPLES_LOADERS_FEATURE_TEST_MODELS_ALPHA_BLEND_MODE_TEST_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Alpha Blend Mode Test Scene (glTF). Tests alpha modes and settings.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AlphaBlendModeTest
 */
struct AlphaBlendModeTestScene : public IRenderableScene {

  AlphaBlendModeTestScene(ICanvas* iCanvas);
  ~AlphaBlendModeTestScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct AlphaBlendModeTestScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_FEATURE_TEST_MODELS_ALPHA_BLEND_MODE_TEST_SCENE_H
