#ifndef BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_BIRD_SCENE_H
#define BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_BIRD_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct PolygonMeshBirdScene : public IRenderableScene {

  PolygonMeshBirdScene(ICanvas* iCanvas);
  ~PolygonMeshBirdScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PolygonMeshBirdScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_BIRD_SCENE_H
