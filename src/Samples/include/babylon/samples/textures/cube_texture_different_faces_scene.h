#ifndef BABYLON_SAMPLES_TEXTURES_CUBE_TEXTURE_DIFFERENT_FACES_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_CUBE_TEXTURE_DIFFERENT_FACES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cube texture different faces scene. Example demonstrating how to
 * create a box with different textures on each face.
 * @see https://www.babylonjs-playground.com/#UU7RQ#2
 */
struct CubeTextureDifferentFacesScene : public IRenderableScene {

  CubeTextureDifferentFacesScene(ICanvas* iCanvas);
  ~CubeTextureDifferentFacesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CubeTextureDifferentFacesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_CUBE_TEXTURE_DIFFERENT_FACES_SCENE_H
