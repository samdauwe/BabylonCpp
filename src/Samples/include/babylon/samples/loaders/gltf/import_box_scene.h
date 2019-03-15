#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Box Scene (glTF). Example demonstrating how to easily load a glTF
 * model.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://www.babylonjs-playground.com/#FY6DP6#0
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Box
 */
struct ImportBoxScene : public IRenderableScene {

  ImportBoxScene(ICanvas* iCanvas);
  ~ImportBoxScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportBoxScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_SCENE_H
