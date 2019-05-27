#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_ANIMATED_MORPH_CUBE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_ANIMATED_MORPH_CUBE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Animated Morph Cube (glTF). The cube contains two morph targets
 * in it:
 * * thin : moves the vertices so that it becomes super-thin
 * * angle : moves the vertices so that it forms a ramp
 * The animation loops, morphing between these two states.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedMorphCube
 */
struct ImportAnimatedMorphCubeScene : public IRenderableScene {

  ImportAnimatedMorphCubeScene(ICanvas* iCanvas);
  ~ImportAnimatedMorphCubeScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportAnimatedMorphCubeScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_ANIMATED_MORPH_CUBE_SCENE_H
