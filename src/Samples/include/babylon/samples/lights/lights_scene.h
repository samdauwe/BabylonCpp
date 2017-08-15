#ifndef BABYLON_SAMPLES_LIGHTS_LIGHTS_SCENE_H
#define BABYLON_SAMPLES_LIGHTS_LIGHTS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class LightsScene : public IRenderableScene {

public:
  LightsScene(ICanvas* iCanvas);
  ~LightsScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  std::vector<PointLight*> _lights;
  std::vector<Mesh*> _lightSpheres;
  float _alpha;

}; // end of class LightsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LIGHTS_LIGHTS_SCENE_H
