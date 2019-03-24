#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_TEXTURED_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_TEXTURED_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Box Textured Scene (glTF). Box with one texture. Start with
 * this to test textures.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxTextured
 */
struct ImportBoxTexturedScene : public IRenderableScene {

  ImportBoxTexturedScene(ICanvas* iCanvas);
  ~ImportBoxTexturedScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportBoxTexturedScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_TEXTURED_SCENE_H
