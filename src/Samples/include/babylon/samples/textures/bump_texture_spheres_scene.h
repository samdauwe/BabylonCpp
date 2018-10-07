#ifndef BABYLON_SAMPLES_TEXTURES_BUMP_TEXTURE_SPHERES_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_BUMP_TEXTURE_SPHERES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Bump Texture Spheres Scene. Example demonstrating how to use normal
 * map to simulate bump on multiple spheres.
 * @see https://www.babylonjs-playground.com/#20OAV9#23
 */
class BumpTextureSpheresScene : public IRenderableScene {

public:
  BumpTextureSpheresScene(ICanvas* iCanvas);
  ~BumpTextureSpheresScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _sphere;

}; // end of class BumpTextureSpheresScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_BUMP_TEXTURE_SPHERES_SCENE_H
