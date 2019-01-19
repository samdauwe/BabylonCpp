#ifndef BABYLON_SAMPLES_TEXTURES_LOCAL_CUBEMAPS_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_LOCAL_CUBEMAPS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class CubeTexture;
using CubeTexturePtr = std::shared_ptr<CubeTexture>;

namespace Samples {

/**
 * @brief Local Cubemaps Scene. This example demonstrates how improve cubemaps
 * with local mode.
 * @see https://www.babylonjs-playground.com/#RNASML#4
 * @see https://doc.babylonjs.com/how_to/reflect#cubetexture
 */
class LocalCubemapsScene : public IRenderableScene {

public:
  LocalCubemapsScene(ICanvas* iCanvas);
  ~LocalCubemapsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  CubeTexturePtr _reflectionTexture;

}; // end of class LocalCubemapsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_LOCAL_CUBEMAPS_SCENE_H
