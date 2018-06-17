#ifndef BABYLON_SAMPLES_EXTENSIONS_POLYHEDRON_POLYHEDRA_SCENE_H
#define BABYLON_SAMPLES_EXTENSIONS_POLYHEDRON_POLYHEDRA_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class PolyhedraScene : public IRenderableScene {

public:
  PolyhedraScene(ICanvas* iCanvas);
  ~PolyhedraScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCamera* _camera;
  PointLight* _pointLight;
  std::vector<Mesh*> _polygons;
  Float32Array _rotations;

}; // end of struct PolyhedraScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_EXTENSIONS_POLYHEDRON_POLYHEDRA_SCENE_H
