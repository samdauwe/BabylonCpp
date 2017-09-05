#ifndef BABYLON_SAMPLES_MESH_RIBBON_SCENE_H
#define BABYLON_SAMPLES_MESH_RIBBON_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class RibbonScene : public IRenderableScene {

public:
  RibbonScene(ICanvas* iCanvas);
  ~RibbonScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class RibbonScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_RIBBON_SCENE_H
