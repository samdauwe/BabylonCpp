#ifndef BABYLON_SAMPLES_MESHES_BASIC_SCENE_H
#define BABYLON_SAMPLES_MESHES_BASIC_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct BasicScene : public IRenderableScene {

  BasicScene(ICanvas* iCanvas);
  ~BasicScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BasicScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_BASIC_SCENE_H
