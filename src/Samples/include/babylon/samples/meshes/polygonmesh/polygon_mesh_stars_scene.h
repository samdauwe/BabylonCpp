#ifndef BABYLON_SAMPLES_MESHES_POLYGON_MESH_POLYGON_MESH_STARS_SCENE_H
#define BABYLON_SAMPLES_MESHES_POLYGON_MESH_POLYGON_MESH_STARS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct PolygonMeshStarsScene : public IRenderableScene {

  PolygonMeshStarsScene(ICanvas* iCanvas);
  ~PolygonMeshStarsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PolygonMeshStarsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_POLYGON_MESH_POLYGON_MESH_STARS_SCENE_H
