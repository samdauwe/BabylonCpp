#ifndef BABYLON_SAMPLES_MESHES_COLORED_RIBBON_SCENE_H
#define BABYLON_SAMPLES_MESHES_COLORED_RIBBON_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Colored Ribbon Scene. Example demonstrating how to make a ribbon using
 * its colors option.
 * @see https://www.babylonjs-playground.com/#FJNR5#296
 * @see https://forum.babylonjs.com/t/ribbon-with-colors-option
 */
struct ColoredRibbonScene : public IRenderableScene {

  ColoredRibbonScene(ICanvas* iCanvas);
  ~ColoredRibbonScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ColoredRibbonScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_COLORED_RIBBON_SCENE_H
