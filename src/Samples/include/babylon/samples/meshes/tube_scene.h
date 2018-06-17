#ifndef BABYLON_SAMPLES_MESH_TUBE_SCENE_H
#define BABYLON_SAMPLES_MESH_TUBE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct TubeScene : public IRenderableScene {

  TubeScene(ICanvas* iCanvas);
  ~TubeScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct TubeScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_TUBE_SCENE_H
