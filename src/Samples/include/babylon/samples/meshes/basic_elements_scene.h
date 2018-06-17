#ifndef BABYLON_SAMPLES_MESHES_BASIC_ELEMENTS_SCENE_H
#define BABYLON_SAMPLES_MESHES_BASIC_ELEMENTS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

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
