#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_TRIANGLE_WITHOUT_INDICES_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_TRIANGLE_WITHOUT_INDICES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Triangle Without Indices Scene (glTF). The simplest possible glTF
 * asset: A single scene with a single node and a single mesh with a single
 * mesh.primitive with a single triangle with a single attribute, without
 * indices and without a material.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/TriangleWithoutIndices
 */
struct TriangleWithoutIndicesScene : public IRenderableScene {

  TriangleWithoutIndicesScene(ICanvas* iCanvas);
  ~TriangleWithoutIndicesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct TriangleWithoutIndicesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_TRIANGLE_WITHOUT_INDICES_SCENE_H
