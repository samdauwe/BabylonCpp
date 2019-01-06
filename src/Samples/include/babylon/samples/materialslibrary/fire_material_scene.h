#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_FIRE_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_FIRE_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class Mesh;
class ShadowGenerator;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using MeshPtr            = std::shared_ptr<Mesh>;
using ShadowGeneratorPtr = std::shared_ptr<ShadowGenerator>;

namespace MaterialsLibrary {
class FireMaterial;
using FireMaterialPtr = std::shared_ptr<FireMaterial>;
} // end of namespace MaterialsLibrary

namespace Samples {

/**
 * @brief Scene demonstrating the use of the fire material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#LR4YHT#0
 */
class FireMaterialScene : public IRenderableScene {

public:
  FireMaterialScene(ICanvas* iCanvas);
  ~FireMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _rootMesh;
  MaterialsLibrary::FireMaterialPtr _fireMaterial;
  ShadowGeneratorPtr _shadowGenerator;
  bool _renderWithShadows;

}; // end of class FireMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_FIRE_MATERIAL_SCENE_H
