#ifndef BABYLON_SAMPLES_MESHES_LORENZ_ATTRACTOR_SCENE_H
#define BABYLON_SAMPLES_MESHES_LORENZ_ATTRACTOR_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class LorenzAttractorScene : public IRenderableScene {

public:
  LorenzAttractorScene(ICanvas* iCanvas);
  ~LorenzAttractorScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  LinesMeshPtr lorenz;

}; // end of class LorenzAttractorScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_LORENZ_ATTRACTOR_SCENE_H
