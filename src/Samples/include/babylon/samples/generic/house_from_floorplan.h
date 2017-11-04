#ifndef BABYLON_SAMPLES_GENERIC_HOUSE_FROM_FLOORPLAN_SCENE_H
#define BABYLON_SAMPLES_GENERIC_HOUSE_FROM_FLOORPLAN_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {
namespace Samples {

struct Door {
  float width;
  float height;
  float left = 0.f;
}; // end of struct Door

struct DoorSpace {
  Door door;
  float left = 0.f;
}; // end of struct DoorSpace

struct Window {
  float width;
  float height;
  float left   = 0.f;
  float bottom = 0.f;
}; // end of struct Window

struct WindowSpace {
  Window window;
  float height;
  float left = 0.f;
  float top  = 0.f;
}; // end of struct WindowSpace

struct Wall {
  Vector3 corner;
  vector_t<DoorSpace> doorSpaces;
  vector_t<WindowSpace> windowSpaces;
}; // end of struct Wall

struct WallData {
  Float32Array positions;
  Uint32Array indices;
}; // end of struct WallData

struct BuildFromPlanOptions {
  Vector4 interiorUV   = Vector4(0, 0, 1, 1);
  Vector4 exteriorUV   = Vector4(0, 0, 1, 1);
  Color4 interiorColor = Color4(1, 1, 1, 1);
  Color4 exteriorColor = Color4(1, 1, 1, 1);
}; // end of struct BuildFromPlanOptions

/**
 * @brief Build a House from Plans scene. Starting with a polygon as the
 * footprint, a house is built by giving the footprint thickness (ply),
 * extruding and adding door and window spaces at given positions.
 *
 * Source: https://doc.babylonjs.com/samples/house
 */
class HouseFromFloorplanScene : public IRenderableScene {

public:
  HouseFromFloorplanScene(ICanvas* iCanvas);
  ~HouseFromFloorplanScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  Mesh* buildFromPlan(vector_t<Wall>& walls, float ply, float height,
                      BuildFromPlanOptions& options, Scene* scene);

private:
  float _alpha;
  Mesh* _house;

}; // end of class HouseFromFloorplanScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_GENERIC_HOUSE_FROM_FLOORPLAN_SCENE_H
