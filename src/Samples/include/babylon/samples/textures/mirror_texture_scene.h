#ifndef BABYLON_SAMPLES_TEXTURES_MIRROR_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_MIRROR_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Mirror Texture Scene. Example demonstrating that MirrorTexture
 * objects within the scene can be reflected as in a mirror.
 * @see http://www.babylonjs-playground.com/#1YAIO7#5
 */
struct MirrorTextureScene : public IRenderableScene {

  MirrorTextureScene(ICanvas* iCanvas);
  ~MirrorTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct MirrorTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_MIRROR_TEXTURE_SCENE_H
