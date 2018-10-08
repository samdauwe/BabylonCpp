#ifndef BABYLON_SAMPLES_MESHES_COLORED_CUBE_FACES_SCENE_H
#define BABYLON_SAMPLES_MESHES_COLORED_CUBE_FACES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Colored Cube Faces Scene. Example demonstrating how to set the color
 * of each face of the cube.
 * @see http://playground.babylonjs.com/#T40FK#0
 */
struct ColoredCubeFacesScene : public IRenderableScene {

  ColoredCubeFacesScene(ICanvas* iCanvas);
  ~ColoredCubeFacesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ColoredCubeFacesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_COLORED_CUBE_FACES_SCENE_H
