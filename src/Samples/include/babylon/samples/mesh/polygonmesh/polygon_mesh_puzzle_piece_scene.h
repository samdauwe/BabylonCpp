#ifndef BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_PUZZLE_PIECE_SCENE_H
#define BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_PUZZLE_PIECE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct PolygonMeshPuzzlePieceScene : public IRenderableScene {

  PolygonMeshPuzzlePieceScene(ICanvas* iCanvas);
  ~PolygonMeshPuzzlePieceScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PolygonMeshPuzzlePieceScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MESH_POLYGON_MESH_POLYGON_MESH_PUZZLE_PIECE_SCENE_H
