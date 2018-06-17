#ifndef BABYLON_SAMPLES_MESHES_EXTRUDED_POLYGON_SCENE_H
#define BABYLON_SAMPLES_MESHES_EXTRUDED_POLYGON_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class ExtrudedPolygonScene : public IRenderableScene {

public:
  ExtrudedPolygonScene(ICanvas* iCanvas);
  ~ExtrudedPolygonScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class ExtrudedPolygonScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_EXTRUDED_POLYGON_SCENE_H
