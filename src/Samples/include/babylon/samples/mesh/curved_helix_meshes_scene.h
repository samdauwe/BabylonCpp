#ifndef BABYLON_SAMPLES_MESH_CURVED_HELIX_MESHES_SCENE_H
#define BABYLON_SAMPLES_MESH_CURVED_HELIX_MESHES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class CurvedHelixMeshesScene : public IRenderableScene {

public:
  CurvedHelixMeshesScene(ICanvas* iCanvas);
  ~CurvedHelixMeshesScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  void _showAxis(float size, Scene* scene);

}; // end of class CurvedHelixMeshesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_CURVED_HELIX_MESHES_SCENE_H
