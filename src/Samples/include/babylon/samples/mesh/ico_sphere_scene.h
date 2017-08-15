#ifndef BABYLON_SAMPLES_MESH_ICO_SPHERE_SCENE_H
#define BABYLON_SAMPLES_MESH_ICO_SPHERE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class IcoSphereScene : public IRenderableScene {

public:
  IcoSphereScene(ICanvas* iCanvas);
  virtual ~IcoSphereScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class IcoSphereScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_ICO_SPHERE_SCENE_H
