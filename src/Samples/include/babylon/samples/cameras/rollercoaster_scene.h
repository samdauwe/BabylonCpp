#ifndef BABYLON_SAMPLES_CAMERAS_ROLLERCOASTER_SCENE_H
#define BABYLON_SAMPLES_CAMERAS_ROLLERCOASTER_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

#include <babylon/math/vector3.h>

namespace BABYLON {

class FollowCamera;
class Mesh;
using FollowCameraPtr = std::shared_ptr<FollowCamera>;
using MeshPtr         = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Rollercoaster Scene. Scene demonstrating a follow camera following a
 * moving target.
 * @see https://www.babylonjs-playground.com/#1HH4OJ#29
 */
class RollercoasterScene : public IRenderableScene {

public:
  RollercoasterScene(ICanvas* iCanvas);
  ~RollercoasterScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  // List with points
  std::vector<Vector3> _points;
  // Points size
  size_t _lg;
  // Tangent vectors
  std::vector<Vector3> _tgts;
  // Normal vectors
  std::vector<Vector3> _norms;
  // Binormal vectors
  std::vector<Vector3> _binorms;
  // Curve current point
  size_t _i;
  // Curve next current point
  size_t _j;
  // Curve current segment
  Vector3 _curvect;
  // Wagon current rotation
  Vector3 _rot;
  // Wagon current position
  Vector3 _pos;
  // Current scaled segment
  Vector3 _scaled;
  // nb steps on each segment
  size_t _step;
  // Counter
  size_t _k;
  // Wagon Mesh
  MeshPtr _wagon;
  // Follow Camera
  FollowCameraPtr _followCam;

}; // end of class RollercoasterScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CAMERAS_ROLLERCOASTER_SCENE_H
