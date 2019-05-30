#ifndef BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Material Scene (glTF).
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/cx20/gltf-test/tree/master/tutorialModels/SimpleMaterial
 */
struct SimpleMaterialGLTFScene : public IRenderableScene {

  SimpleMaterialGLTFScene(ICanvas* iCanvas);
  ~SimpleMaterialGLTFScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SimpleMaterialGLTFScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_SIMPLE_MATERIAL_SCENE_H
