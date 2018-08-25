#ifndef BABYLON_SAMPLES_MATERIALS_MULTI_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MULTI_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Multi material scene. Example demonstrating how to apply multiple
 * materials to a single mesh using MultiMaterial class
 * @see https://www.babylonjs-playground.com/#2Q4S2S#0
 */
class MultiMaterialScene : public IRenderableScene {

public:
  MultiMaterialScene(ICanvas* iCanvas);
  ~MultiMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _sphere;

}; // end of class MultiMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_MULTI_MATERIAL_SCENE_H
