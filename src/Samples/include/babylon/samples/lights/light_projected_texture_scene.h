#ifndef BABYLON_SAMPLES_LIGHTS_LIGHT_PROJECTED_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_LIGHTS_LIGHT_PROJECTED_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Light projected texture scene. Example demonstrating how to project a
 * texture from a spot light.
 * @see https://www.babylonjs-playground.com/#CQNGRK#0
 */
class LightProjectedTextureScene : public IRenderableScene {

public:
  LightProjectedTextureScene(ICanvas* iCanvas);
  ~LightProjectedTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  SpotLight* _spotLight;
  float _alpha;

}; // end of class LightProjectedTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LIGHTS_LIGHT_PROJECTED_TEXTURE_SCENE_H
