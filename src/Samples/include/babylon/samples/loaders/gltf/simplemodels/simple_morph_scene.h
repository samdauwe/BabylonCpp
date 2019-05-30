#ifndef BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_MORPH_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_MORPH_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Morph Scene (glTF). A triangle with a morph animation applied.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/SimpleMorph
 */
struct SimpleMorphScene : public IRenderableScene {

  SimpleMorphScene(ICanvas* iCanvas);
  ~SimpleMorphScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SimpleMorphScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_MORPH_SCENE_H
