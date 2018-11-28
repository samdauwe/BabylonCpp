#ifndef BABYLON_SAMPLES_TEXTURES_HDR_CUBE_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_HDR_CUBE_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief High Dynamic Range (HDR) cube texture scene. Example demonstrating how
 * to create a HDR skybox. High Dynamic Range (HDR) images are panoramic images
 * that cover an entire field of vision..
 * @see http://www.babylonjs-playground.com/#114YPX#5
 */
struct HDRCubeTextureScene : public IRenderableScene {

  HDRCubeTextureScene(ICanvas* iCanvas);
  ~HDRCubeTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct HDRCubeTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_HDR_CUBE_TEXTURE_SCENE_H
