#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_INTERLEAVED_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_INTERLEAVED_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Box Interleaved Scene (glTF). Box example with interleaved
 * position and normal attributes.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxInterleaved
 */
struct ImportBoxInterleavedScene : public IRenderableScene {

  ImportBoxInterleavedScene(ICanvas* iCanvas);
  ~ImportBoxInterleavedScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportBoxInterleavedScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_INTERLEAVED_SCENE_H
