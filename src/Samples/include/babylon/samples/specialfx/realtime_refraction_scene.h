#ifndef BABYLON_SAMPLES_SPECIAL_FX_REALTIME_REFRACTION_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_REALTIME_REFRACTION_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Realtime refraction scene. This example demonstrates how to use
 * reflection probes to simulate realtime refraction.
 * @see https://www.babylonjs-playground.com/#RRYXWN#0
 */
class RealtimeRefractionScene : public IRenderableScene {

public:
  RealtimeRefractionScene(ICanvas* iCanvas);
  ~RealtimeRefractionScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

public:
  MeshPtr _yellowSphere;
  MeshPtr _greenSphere;

}; // end of class RealtimeRefractionScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_REALTIME_REFRACTION_SCENE_H
