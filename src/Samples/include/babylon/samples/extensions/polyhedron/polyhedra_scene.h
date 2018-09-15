#ifndef BABYLON_SAMPLES_EXTENSIONS_POLYHEDRON_POLYHEDRA_SCENE_H
#define BABYLON_SAMPLES_EXTENSIONS_POLYHEDRON_POLYHEDRA_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class Mesh;
class PointLight;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using MeshPtr            = std::shared_ptr<Mesh>;
using PointLightPtr      = std::shared_ptr<PointLight>;

namespace Samples {

class PolyhedraScene : public IRenderableScene {

public:
  PolyhedraScene(ICanvas* iCanvas);
  ~PolyhedraScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;
  PointLightPtr _pointLight;
  std::vector<MeshPtr> _polygons;
  Float32Array _rotations;

}; // end of struct PolyhedraScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_EXTENSIONS_POLYHEDRON_POLYHEDRA_SCENE_H
