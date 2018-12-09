#ifndef BABYLON_SAMPLES_TEXTURES_SPHERE_TRANSPARENT_TEXTURES_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_SPHERE_TRANSPARENT_TEXTURES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Sphere Transparent Textures Scene. Example demonstrating how to
 * achieve transparency by setting a materials alpha property from 0 (invisible)
 * to 1 (opaque).
 * @see http://www.babylonjs-playground.com/#20OAV9#17
 */
struct SphereTransparentTexturesScene : public IRenderableScene {

  SphereTransparentTexturesScene(ICanvas* iCanvas);
  ~SphereTransparentTexturesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SphereTransparentTexturesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_SPHERE_TRANSPARENT_TEXTURES_SCENE_H
