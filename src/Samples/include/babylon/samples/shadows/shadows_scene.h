#ifndef BABYLON_SAMPLES_SHADOWS_SHADOWS_SCENE_H
#define BABYLON_SAMPLES_SHADOWS_SHADOWS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Shadows Scene. Example scene demonstrating how to setup a scene with
 * lights and meshes to cast different types of shadows.
 * @see https://www.babylonjs-playground.com/#IFYDRS#0
 */
class ShadowsScene : public IRenderableScene {

public:
  ShadowsScene(ICanvas* iCanvas);
  ~ShadowsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _torus;
  float _alpha;

}; // end of class ShadowsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SHADOWS_SHADOWS_SCENE_H
