#ifndef BABYLON_SAMPLES_MESH_POLYHEDRON_SCENE_H
#define BABYLON_SAMPLES_MESH_POLYHEDRON_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class PointLight;

namespace Samples {

class PolyhedronScene : public IRenderableScene {

public:
  PolyhedronScene(ICanvas* iCanvas);
  ~PolyhedronScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCamera* _camera;
  PointLight* _pointLight;
  std::vector<Mesh*> _polygons;
  std::vector<float> _rotations;

}; // end of class PolyhedronScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_POLYHEDRON_SCENE_H
