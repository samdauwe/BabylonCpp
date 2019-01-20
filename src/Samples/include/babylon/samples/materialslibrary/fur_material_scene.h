#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_FUR_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_FUR_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class AbstractMesh;
class ArcRotateCamera;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;

namespace Samples {

/**
 * @brief Scene demonstrating the use of the fur material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#VABI8A#0
 * @see https://doc.babylonjs.com/extensions/fur
 */
class FurMaterialScene : public IRenderableScene {

public:
  FurMaterialScene(ICanvas* iCanvas);
  ~FurMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  void _configureFur(const AbstractMeshPtr& mesh, Scene* scene) const;

private:
  ArcRotateCameraPtr _camera;

}; // end of class FurMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_FUR_MATERIAL_SCENE_H
