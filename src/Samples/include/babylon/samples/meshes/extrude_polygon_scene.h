#ifndef BABYLON_SAMPLES_MESHES_EXTRUDE_POLYGON_SCENE_H
#define BABYLON_SAMPLES_MESHES_EXTRUDE_POLYGON_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Extrude Polygon Scene. Example demonstrating how to use MeshBuilder to
 * generate geometry from extruded data.
 * Press the following keys to toggle visibility:
 * - 1: Roof
 * - 2: Front
 * - 3: Back
 * - 4: Left Side
 * - 5: Right Side
 * @see https://www.babylonjs-playground.com/#TFLTJJ#0
 */
class ExtrudePolygonScene : public IRenderableScene {

public:
  ExtrudePolygonScene(ICanvas* iCanvas);
  ~ExtrudePolygonScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  // Roof
  MeshPtr _roof, _ceiling;
  // Front
  MeshPtr _frontWall, _windowFBL, _windowFBR, _windowFTL, _windowFTR,
    _windowFTM, _frontDoor;
  // Back
  MeshPtr _rearWallnb1, _rearWallnb2, _windowRBL, _windowRBR, _windowRTL,
    _windowRTR, _windowR1BL, _windowR1TL, _windowR1TR;
  // Left Side
  MeshPtr _sideWallnb1, _sideWallnb3, _backDoor;
  // Right Side
  MeshPtr _sideWallnb2;

}; // end of class ExtrudePolygonScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_EXTRUDE_POLYGON_SCENE_H
