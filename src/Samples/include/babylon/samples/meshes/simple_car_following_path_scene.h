#ifndef BABYLON_SAMPLES_MESHES_SIMPLE_CAR_FOLLOWING_PATH_SCENE_H
#define BABYLON_SAMPLES_MESHES_SIMPLE_CAR_FOLLOWING_PATH_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/math/quaternion.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Simple Car Following Path. The car will consist of a trapezoid for the
 * body, four cylinders for wheels. The car will be animated to follow a path an
 * remain parallel to it. The wheels will rotate and be given a texture to show
 * that they are rotating. For simplicity all wheels will remain parallel to the
 * car and their rotation will not be matched to the forward speed of the car.
 * The car will move forward by following a sequence of points on the path. The
 * path will be a curved line with large turning radius and will be shown in the
 * scene.
 *
 * Source: https://doc.babylonjs.com/samples/car_path
 */
class SimpleCarFollowingPathScene : public IRenderableScene {

public:
  SimpleCarFollowingPathScene(ICanvas* iCanvas);
  ~SimpleCarFollowingPathScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  unsigned int _curveNumberOfPoints;
  unsigned int _iteration;
  Quaternion _startRotation;
  MeshPtr _carBody;
  MeshPtr _wheelFI, _wheelFO, _wheelRI, _wheelRO;
  std::vector<Vector3> _points, _normals;

}; // end of class SimpleCarFollowingPathScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_SIMPLE_CAR_FOLLOWING_PATH_SCENE_H
