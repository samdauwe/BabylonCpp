#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRADIENT_MATERIAL_SPHERE_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRADIENT_MATERIAL_SPHERE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Scene demonstrating the use of the gradient material from the
 * materials library.
 */
class GradientMaterialSphereScene : public IRenderableScene {

public:
  GradientMaterialSphereScene(ICanvas* iCanvas);
  ~GradientMaterialSphereScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _sphere;
  float _alpha;

}; // end of struct GradientMaterialSphereScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRADIENT_MATERIAL_SPHERE_SCENE_H
