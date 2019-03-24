#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_DUCK_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_DUCK_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Duck Scene (glTF). The COLLADA duck. One texture.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Duck
 */
struct ImportDuckScene : public IRenderableScene {

  ImportDuckScene(ICanvas* iCanvas);
  ~ImportDuckScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportDuckScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_DUCK_SCENE_H
