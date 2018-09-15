#ifndef BABYLON_SAMPLES_TEXTURES_BUMP_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_BUMP_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Bump Texture Scene. Example demonstrating how to use normal map to
 * simulate bump.
 * @see https://www.babylonjs-playground.com/#RK0W5S#0
 */
class BumpTextureScene : public IRenderableScene {

public:
  BumpTextureScene(ICanvas* iCanvas);
  ~BumpTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _sphere;

}; // end of class BumpTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_BUMP_TEXTURE_SCENE_H
