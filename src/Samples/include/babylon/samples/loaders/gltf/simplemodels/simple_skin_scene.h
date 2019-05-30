#ifndef BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_SKIN_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_SKIN_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Skin Scene (glTF).
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/cx20/gltf-test/tree/master/tutorialModels/SimpleSkin
 */
struct SimpleSkinScene : public IRenderableScene {

  SimpleSkinScene(ICanvas* iCanvas);
  ~SimpleSkinScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SimpleSkinScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_SKIN_SCENE_H
