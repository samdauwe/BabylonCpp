#ifndef BABYLON_SAMPLES_MESH_EXTRUSION_SCENE_H
#define BABYLON_SAMPLES_MESH_EXTRUSION_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct ExtrusionScene : public IRenderableScene {

  ExtrusionScene(ICanvas* iCanvas);
  ~ExtrusionScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ExtrusionScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_EXTRUSION_SCENE_H
