#ifndef BABYLON_SAMPLES_MESHES_POLYHEDRON_SCENE_H
#define BABYLON_SAMPLES_MESHES_POLYHEDRON_SCENE_H

#include <vector>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class Mesh;
class PointLight;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using MeshPtr            = std::shared_ptr<Mesh>;
using PointLightPtr      = std::shared_ptr<PointLight>;

namespace Samples {

class PolyhedronScene : public IRenderableScene {

public:
  PolyhedronScene(ICanvas* iCanvas);
  ~PolyhedronScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;
  PointLightPtr _pointLight;
  std::vector<MeshPtr> _polygons;
  std::vector<float> _rotations;

}; // end of class PolyhedronScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_POLYHEDRON_SCENE_H
