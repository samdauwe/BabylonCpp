#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRADIENT_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRADIENT_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the gradient material from the
 * materials library.
 * @see https://www.babylonjs-playground.com/#2IFRKC#22
 */
struct GradientMaterialScene : public IRenderableScene {

  GradientMaterialScene(ICanvas* iCanvas);
  ~GradientMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct GradientMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_GRADIENT_MATERIAL_SCENE_H
