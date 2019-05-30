#ifndef BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_TRIANGLE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_TRIANGLE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Triangle Scene (glTF). A very simple glTF asset: The basic structure
 * is the same as in Triangle Without Indices, but here, the mesh.primitive
 * describes an indexed geometry.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Triangle
 */
struct TriangleScene : public IRenderableScene {

  TriangleScene(ICanvas* iCanvas);
  ~TriangleScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct TriangleScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_TRIANGLE_SCENE_H
