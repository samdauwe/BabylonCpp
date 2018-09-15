#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_TRI_PLANAR_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_TRI_PLANAR_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class GroundMesh;
class StandardMaterial;
using GroundMeshPtr       = std::shared_ptr<GroundMesh>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

namespace MaterialsLibrary {
class TriPlanarMaterial;
using TriPlanarMaterialPtr = std::shared_ptr<TriPlanarMaterial>;
} // end of namespace MaterialsLibrary

namespace Samples {

/**
 * @brief Scene demonstrating the use of the Tri-Planar material from the
 * materials library.
 * Press the following keys:
 * - 1: to activate the Tri-Planar material (by default)
 * - 2: to active the default material
 * @see https://www.babylonjs-playground.com/#E6OZX#9
 */
class TriPlanarMaterialScene : public IRenderableScene {

public:
  TriPlanarMaterialScene(ICanvas* iCanvas);
  ~TriPlanarMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MaterialsLibrary::TriPlanarMaterialPtr _triPlanarMaterial;
  StandardMaterialPtr _defaultMaterial;
  GroundMeshPtr _ground;

}; // end of class TriPlanarMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_TRI_PLANAR_MATERIAL_SCENE_H
