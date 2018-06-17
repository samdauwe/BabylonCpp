#ifndef BABYLON_SAMPLES_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_SCENE_H
#define BABYLON_SAMPLES_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct SimplePineScene : public IRenderableScene {

  SimplePineScene(ICanvas* iCanvas);
  ~SimplePineScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SimplePineScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_SCENE_H
