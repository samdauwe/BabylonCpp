#ifndef BABYLON_SAMPLES_MESHES_BASIC_ELEMENTS_SCENE_H
#define BABYLON_SAMPLES_MESHES_BASIC_ELEMENTS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Basic elements scene. Example demonstrating how to create Ball, box,
 * plane, line, etc.
 * @see https://www.babylonjs-playground.com/#A1210C#0
 * @see https://doc.babylonjs.com/how_to/set_shapes
 */
class BasicElementsScene : public IRenderableScene {

public:
  BasicElementsScene(ICanvas* iCanvas);
  ~BasicElementsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class BasicElementsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_BASIC_ELEMENTS_SCENE_H
