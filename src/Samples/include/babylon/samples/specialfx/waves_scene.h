#ifndef BABYLON_SAMPLES_SPECIAL_FX_WAVES_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_WAVES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;

namespace Samples {

class WavesScene : public IRenderableScene {

public:
  WavesScene(ICanvas* iCanvas);
  ~WavesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _ground;
  MeshPtr _ground2;
  Float32Array _positions;
  Uint32Array _indices;
  float _alpha;

}; // end of class WavesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_WAVES_SCENE_H
