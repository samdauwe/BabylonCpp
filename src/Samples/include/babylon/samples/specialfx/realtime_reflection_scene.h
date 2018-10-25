#ifndef BABYLON_SAMPLES_SPECIAL_FX_REALTIME_REFLECTION_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_REALTIME_REFLECTION_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Realtime reflection scene. This example demonstrates how to use
 * reflection probes to simulate realtime reflection.
 * @see https://www.babylonjs-playground.com/#J0D279#0
 */
class RealtimeReflectionScene : public IRenderableScene {

public:
  RealtimeReflectionScene(ICanvas* iCanvas);
  ~RealtimeReflectionScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

public:
  MeshPtr _yellowSphere;
  MeshPtr _greenSphere;
  MeshPtr _blueSphere;

}; // end of class RealtimeReflectionScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_REALTIME_REFLECTION_SCENE_H
