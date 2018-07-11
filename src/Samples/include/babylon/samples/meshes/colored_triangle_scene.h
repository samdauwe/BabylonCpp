#ifndef BABYLON_SAMPLES_MESHES_COLORED_TRIANGLE_SCENE_H
#define BABYLON_SAMPLES_MESHES_COLORED_TRIANGLE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Colored Triangle Scene.
 * @see https://www.babylonjs-playground.com/#WTKKF6#4
 */
struct ColoredTriangleScene : public IRenderableScene {

  ColoredTriangleScene(ICanvas* iCanvas);
  ~ColoredTriangleScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ColoredTriangleScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_COLORED_TRIANGLE_SCENE_H
