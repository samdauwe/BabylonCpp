#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_MESHES_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_MESHES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Simple Meshes Scene (glTF). This is an example showing how the
 * same mesh may be appended to several nodes, to be rendered multiple times. It
 * also shows how additional vertex attributes are defined in a mesh.primitive -
 * namely, attributes for the vertex normals.
 * @note The additional vertex normal attribute in this example is not yet used
 * by any technique. This may cause a warning to be printed during the
 * validation. The normal attribute will be used in the AdvancedMaterial
 * example.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/SimpleMeshes
 */
struct ImportSimpleMeshesScene : public IRenderableScene {

  ImportSimpleMeshesScene(ICanvas* iCanvas);
  ~ImportSimpleMeshesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportSimpleMeshesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_MESHES_SCENE_H
