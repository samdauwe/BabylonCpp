#ifndef BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_RECIPROCATING_SAW_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_RECIPROCATING_SAW_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Reciprocating Saw Scene (glTF). Small CAD data set, including
 * hierarchy.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Duck
 */
struct ReciprocatingSawScene : public IRenderableScene {

  ReciprocatingSawScene(ICanvas* iCanvas);
  ~ReciprocatingSawScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ReciprocatingSawScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_RECIPROCATING_SAW_SCENE_H
