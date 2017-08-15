#ifndef BABYLON_SAMPLES_MESH_MESH_NORMALS_SCENE_H
#define BABYLON_SAMPLES_MESH_MESH_NORMALS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct MeshNormalsScene : public IRenderableScene {

  MeshNormalsScene(ICanvas* iCanvas);
  ~MeshNormalsScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct MeshNormalsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_MESH_NORMALS_SCENE_H
