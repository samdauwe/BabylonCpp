#ifndef BABYLON_SAMPLES_MESHES_LINES_MESH_SPIRAL_SCENE_H
#define BABYLON_SAMPLES_MESHES_LINES_MESH_SPIRAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class LinesMeshSpiralScene : public IRenderableScene {

public:
  LinesMeshSpiralScene(ICanvas* iCanvas);
  ~LinesMeshSpiralScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  LinesMesh* _circle;

}; // end of class LinesMeshSpiralScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_LINES_MESH_SPIRAL_SCENE_H
