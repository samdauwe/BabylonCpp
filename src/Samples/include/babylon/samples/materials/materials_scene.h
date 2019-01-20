#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Materials scene. Example demonstrating how to create and add materials
 * to a mesh.
 * @see https://www.babylonjs-playground.com/#DXARSP#0
 * @see https://doc.babylonjs.com/babylon101/materials
 */
struct MaterialsScene : public IRenderableScene {

  MaterialsScene(ICanvas* iCanvas);
  ~MaterialsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct MaterialsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_MATERIALS_SCENE_H
