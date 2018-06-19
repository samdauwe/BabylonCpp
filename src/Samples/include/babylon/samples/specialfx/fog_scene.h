#ifndef BABYLON_SAMPLES_SPECIAL_FX_FOG_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_FOG_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Fog scene. This example demonstrates how to simulate fog in your
 * scene.
 * @see https://www.babylonjs-playground.com/#LR6389#0
 */
class FogScene : public IRenderableScene {

public:
  FogScene(ICanvas* iCanvas);
  ~FogScene() override;

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

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_FOG_SCENE_H
