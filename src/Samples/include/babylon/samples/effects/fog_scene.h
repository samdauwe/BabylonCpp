#ifndef BABYLON_SAMPLES_EFFECTS_FOG_SCENE_H
#define BABYLON_SAMPLES_EFFECTS_FOG_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class FogScene : public IRenderableScene {

public:
  FogScene(ICanvas* iCanvas);
  ~FogScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _alpha;
  Mesh* _sphere0;
  Mesh* _sphere1;
  Mesh* _sphere2;

}; // end of class FogScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_EFFECTS_FOG_SCENE_H
