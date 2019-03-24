#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_RECIPROCATING_SAW_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_RECIPROCATING_SAW_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Reciprocating Saw Scene (glTF). Small CAD data set, including
 * hierarchy.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Duck
 */
struct ImportReciprocatingSawScene : public IRenderableScene {

  ImportReciprocatingSawScene(ICanvas* iCanvas);
  ~ImportReciprocatingSawScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportReciprocatingSawScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_RECIPROCATING_SAW_SCENE_H
