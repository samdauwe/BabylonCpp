#ifndef BABYLON_SAMPLES_SHADOWS_BOX_SHADOW_SCENE_H
#define BABYLON_SAMPLES_SHADOWS_BOX_SHADOW_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Box Shadow Scene. Example scene demonstrating how to setup a scene
 * with q light and box mesh to cast a shadow.
 */
struct BoxShadowScene : public IRenderableScene {

  BoxShadowScene(ICanvas* iCanvas);
  ~BoxShadowScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BoxShadowScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SHADOWS_BOX_SHADOW_SCENE_H
