#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_VERTEX_COLORS_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_VERTEX_COLORS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Box Vertex Colors Scene (glTF). Box with vertex colors applied.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxVertexColors
 */
struct ImportBoxVertexColorsScene : public IRenderableScene {

  ImportBoxVertexColorsScene(ICanvas* iCanvas);
  ~ImportBoxVertexColorsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportBoxVertexColorsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_BOX_VERTEX_COLORS_SCENE_H
