#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_ANIMATED_TRIANGLE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_ANIMATED_TRIANGLE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Animated Triangle Scene (glTF). This sample is similar to the
 * Triangle, but the node has a rotation property that is modified with a simple
 * animation.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Triangle
 */
struct ImportAnimatedTriangleScene : public IRenderableScene {

  ImportAnimatedTriangleScene(ICanvas* iCanvas);
  ~ImportAnimatedTriangleScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportAnimatedTriangleScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_ANIMATED_TRIANGLE_SCENE_H
