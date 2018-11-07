#ifndef BABYLON_SAMPLES_TEXTURES_MIRRORS_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_MIRRORS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Mirrors Scene. Example demonstrating how to use mirrors.
 * @see https://www.babylonjs-playground.com/#2EP7UB#0
 */
class MirrorsScene : public IRenderableScene {

public:
  MirrorsScene(ICanvas* iCanvas);
  ~MirrorsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _box1;
  float _step;

}; // end of class MirrorsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_MIRRORS_SCENE_H
