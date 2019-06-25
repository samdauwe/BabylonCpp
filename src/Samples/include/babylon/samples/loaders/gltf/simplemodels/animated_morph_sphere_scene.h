#ifndef BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_ANIMATED_MORPH_SPHERE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_ANIMATED_MORPH_SPHERE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Animated Morph Sphere Scene (glTF). The sphere contains two morph
 * targets in it:
 * * blob : moves the vertices so that it becomes a somewhat amorphous fat, blob
 *          shape.
 * * ship : moves the vertices so that it looks a bit like a spaceship, the goal
 *          being to just make it a much more complicated morph example than the
 *          cube.
 * The animation loops, morphing between these two states.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedMorphSphere
 */
struct AnimatedMorphSphereScene : public IRenderableScene {

  AnimatedMorphSphereScene(ICanvas* iCanvas);
  ~AnimatedMorphSphereScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct AnimatedMorphSphereScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_SIMPLE_MODELS_ANIMATED_MORPH_SPHERE_SCENE_H
