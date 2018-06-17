#ifndef BABYLON_SAMPLES_MESHES_POLYGON_MESH_POLYGON_MESH_STARS_WITH_HOLES_SCENE_H
#define BABYLON_SAMPLES_MESHES_POLYGON_MESH_POLYGON_MESH_STARS_WITH_HOLES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct PolygonMeshStarsWithHolesScene : public IRenderableScene {

  PolygonMeshStarsWithHolesScene(ICanvas* iCanvas);
  ~PolygonMeshStarsWithHolesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PolygonMeshStarsWithHolesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MESHES_POLYGON_MESH_POLYGON_MESH_STARS_WITH_HOLES_SCENE_H
