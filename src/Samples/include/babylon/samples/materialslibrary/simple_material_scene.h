#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SIMPLE_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SIMPLE_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the simple material from the materials
 * library.
 */
struct SimpleMaterialScene : public IRenderableScene {

  SimpleMaterialScene(ICanvas* iCanvas);
  ~SimpleMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SimpleMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SIMPLE_MATERIAL_SCENE_H
