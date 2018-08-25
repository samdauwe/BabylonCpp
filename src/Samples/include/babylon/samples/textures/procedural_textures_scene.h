#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Procedural Textures Scene. Example demonstrating how to use procedual
 * textures for wood, grass, marble, fire, etc.
 * @see https://www.babylonjs-playground.com/#B2ZXG6#0
 */
class ProceduralTexturesScene : public IRenderableScene {

public:
  ProceduralTexturesScene(ICanvas* iCanvas);
  ~ProceduralTexturesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;

}; // end of class ProceduralTexturesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_PROCEDURAL_TEXTURES_SCENE_H
