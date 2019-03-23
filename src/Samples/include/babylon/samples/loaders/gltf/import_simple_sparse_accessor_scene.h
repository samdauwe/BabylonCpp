#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_SPARSE_ACCESSOR_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_SPARSE_ACCESSOR_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Sparse Accessor Scene (glTF). A simple mesh that uses sparse
 * accessors.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/SimpleSparseAccessor
 */
struct ImportSimpleSparseAccessorScene : public IRenderableScene {

  ImportSimpleSparseAccessorScene(ICanvas* iCanvas);
  ~ImportSimpleSparseAccessorScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportSimpleSparseAccessorScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_SPARSE_ACCESSOR_SCENE_H
