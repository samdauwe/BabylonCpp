#ifndef BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_MONKEY_SCENE_H
#define BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_MONKEY_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct PolygonMeshMonkeyScene : public IRenderableScene {

  PolygonMeshMonkeyScene(ICanvas* iCanvas);
  ~PolygonMeshMonkeyScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PolygonMeshMonkeyScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_MONKEY_SCENE_H
