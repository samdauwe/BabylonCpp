#ifndef BABYLON_SAMPLES_TEXTURES_SPHERE_TEXTURES_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_SPHERE_TEXTURES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Sphere Textures Scene. Example demonstrating 3 spheres that are lit by
 * the same hemispheric light, with diffuse red and groundColor green. The first
 * sphere has a diffuse texture, the middle an emissive texture and the one on
 * the right has material with red diffuse color and an ambient texture.
 * @see https://www.babylonjs-playground.com/#20OAV9#15
 */
struct SphereTexturesScene : public IRenderableScene {

  SphereTexturesScene(ICanvas* iCanvas);
  ~SphereTexturesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SphereTexturesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_SPHERE_TEXTURES_SCENE_H
