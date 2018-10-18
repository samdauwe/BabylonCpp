#ifndef BABYLON_SAMPLES_MESHES_EXTRUDE_POLYGON_SCENE_H
#define BABYLON_SAMPLES_MESHES_EXTRUDE_POLYGON_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Extrude Polygon Scene. Example demonstrating how to use MeshBuilder to
 * generate geometry from extruded data.
 * @see https://www.babylonjs-playground.com/#TFLTJJ#0
 */
class ExtrudePolygonScene : public IRenderableScene {

public:
  ExtrudePolygonScene(ICanvas* iCanvas);
  ~ExtrudePolygonScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class ExtrudePolygonScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_EXTRUDE_POLYGON_SCENE_H
