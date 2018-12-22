#ifndef BABYLON_SAMPLES_SPECIAL_POINTS_CLOUD_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_POINTS_CLOUD_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Points Cloud Scene. This example demonstrates how update the vertices
 * data of a mesh to simulate a point cloud.
 * @see https://www.babylonjs-playground.com/#WZNWVG#14
 */
class PointsCloudScene : public IRenderableScene {

public:
  PointsCloudScene(ICanvas* iCanvas);
  ~PointsCloudScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  size_t _pointsCount;
  Float32Array _positions;
  MeshPtr _customMesh;

}; // end of struct LensFlaresScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_POINTS_CLOUD_SCENE_H
